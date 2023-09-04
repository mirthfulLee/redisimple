#include "hash_map.h"

#include <pthread.h>

#include <algorithm>
#include <memory>

#include "redisimple/config.h"
#include "redisimple/util/random.h"
#include "redisimple_data_structure.h"

using redisimple::Config;
namespace redisimple::object::structure {

HashTable::HashTable(int size)
    : entry_num_(0), table_size_(size), size_mask_(size - 1) {
  table_.reset(new std::unique_ptr<TableEntry>[size]);
  memset(table_.get(), 0, size);
}

std::unique_ptr<TableEntry>& HashTable::find(unsigned int bucket,
                                             RedisimpleDataStructure* target) {
  std::unique_ptr<TableEntry>* ptr = table_.get() + bucket;
  if (*ptr == nullptr) {
    return *ptr;
  } else {
    // loop until ptr points to the matched entry or nullptr;
    while (ptr->get()->next_ &&
           target->compare(ptr->get()->next_->value_.get()) != 0) {
      ptr = &(ptr->get()->next_);
    }
    return ptr->get()->next_;
  }
}

int HashTable::add_pair(unsigned int bucket,
                        std::unique_ptr<RedisimpleDataStructure>& key,
                        std::unique_ptr<RedisimpleDataStructure>& value) {
  std::unique_ptr<TableEntry>& target_entry = find(bucket, key.get());
  if (target_entry == nullptr) {
    // target entry not exist, so add new entry
    target_entry.reset(new TableEntry(key, value));
    ++entry_num_;
    return 1;
  }
  return 0;
}

int HashTable::replace_pair(unsigned int bucket,
                            std::unique_ptr<RedisimpleDataStructure>& key,
                            std::unique_ptr<RedisimpleDataStructure>& value) {
  std::unique_ptr<TableEntry>& target_entry = find(bucket, key.get());
  if (target_entry == nullptr) {
    // target entry not exist, so add new entry
    target_entry.reset(new TableEntry(key, value));
    ++entry_num_;
    return 1;
  } else {
    // move the ownership to target entry
    target_entry->value_.reset(value.release());
    return 0;
  }
}

// TODO: how to get random number??
TableEntry* HashTable::get_random_pair() {
  unsigned int bucket = redisimple::util::randint() & size_mask_;
  TableEntry* result = nullptr;
  while (result == nullptr) {
    result = (table_.get() + bucket)->get();
    bucket += redisimple::util::randint() & size_mask_;
  }
  return result;
}

RedisimpleDataStructure* HashTable::get_value(unsigned int bucket,
                                              RedisimpleDataStructure* key) {
  std::unique_ptr<TableEntry>& target_entry = find(bucket, key);
  if (target_entry == nullptr)
    return nullptr;
  else
    return target_entry->value_.get();
}

int HashTable::delete_pair(unsigned int bucket, RedisimpleDataStructure* key) {
  std::unique_ptr<TableEntry>& target_entry = find(bucket, key);
  if (target_entry != nullptr) {
    if (target_entry->next_)
      target_entry.reset(target_entry->next_.release());
    else
      target_entry = nullptr;
    --entry_num_;
    return -1;
  }
  return 0;
}

HashMap::HashMap()
    : hash_table_(new HashTable(8)), expand_table_(), rehash_index_(-1) {}

HashMap::HashMap(int size)
    : hash_table_(new HashTable(size)), expand_table_(), rehash_index_(-1) {}

int HashMap::add_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                      std::unique_ptr<RedisimpleDataStructure>& value) {
  int hash_val = key->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashTable* target_table;
  if (rehash_index_ >= 0) {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  } else {
    target_table = hash_table_.get();
  }
  int result = target_table->add_pair(bucket, key, value);
  check_load_factor();
  return result;
}
// if the key is in dict, replace the value
// else add the pair to dict
int HashMap::replace_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                          std::unique_ptr<RedisimpleDataStructure>& value) {
  int hash_val = key->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashTable* target_table;
  if (rehash_index_ <= bucket) {
    target_table = hash_table_.get();
  } else {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  }
  int result = target_table->replace_pair(bucket, key, value);
  check_load_factor();
  return result;
}

TableEntry* HashMap::get_random_pair() {
  if (rehash_index_ >= 0)
    return hash_table_->get_random_pair();
  else
    return expand_table_->get_random_pair();
}

RedisimpleDataStructure* HashMap::get_value(RedisimpleDataStructure* key) {
  int hash_val = key->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashTable* target_table;
  if (rehash_index_ <= bucket) {
    target_table = hash_table_.get();
  } else {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  }
  auto result = target_table->find(bucket, key)->value_.get();
  return result;
}

int HashMap::delete_pair(RedisimpleDataStructure* key) {
  int hash_val = key->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashTable* target_table;
  if (rehash_index_ >= 0) {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  } else {
    target_table = hash_table_.get();
  }
  int result = target_table->delete_pair(bucket, key);
  check_load_factor();
  return result;
}

void HashMap::clear() {
  if (rehash_index_ >= 0) {
    rehash_index_ = -1;
    expand_table_ = nullptr;
  }
  hash_table_.reset(new HashTable(8));
}
// check load factor of hash_table, decide whether start rehash
void HashMap::check_load_factor() {
  if (rehash_index_ >= 0) {
    rehash();
    return;
  }
  float load_factor = hash_table_->entry_num_ / hash_table_->table_size_;
  // TODO: add conditions cosidering BGSAVE & BGREWRITEAOF
  if (load_factor > 2) {
    rehash_index_ = 0;
    expand_table_.reset(new HashTable(hash_table_->table_size_ << 1));
  } else if (load_factor < 0.1 && hash_table_->table_size_ > 8) {
    rehash_index_ = 0;
    expand_table_.reset(new HashTable(hash_table_->table_size_ >> 1));
  }
  rehash();
}

void HashMap::rehash() {
  int end_idx =
      std::min(Config::rehash_stride + rehash_index_, hash_table_->table_size_);
  for (; rehash_index_ < end_idx; ++rehash_index_) {
    std::unique_ptr<TableEntry>* old_list =
        hash_table_->table_.get() + rehash_index_;
    // move all entry in hash table to expand table
    while (*old_list != nullptr) {
      std::unique_ptr<TableEntry> first_entry(old_list->release());
      old_list->reset(first_entry->next_.release());
      unsigned int new_bucket =
          first_entry->key_->hash() & expand_table_->size_mask_;
      std::unique_ptr<TableEntry>* new_list =
          expand_table_->table_.get() + new_bucket;
      // push first entry to the front of new list
      first_entry.reset(new_list->release());
      new_list->reset(first_entry.release());
    }
  }
  if (rehash_index_ == hash_table_->table_size_) {
    // rehash finished
    hash_table_.reset(expand_table_.release());
    rehash_index_ = -1;
  }
}
}  // namespace redisimple::object::structure