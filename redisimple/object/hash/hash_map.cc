#include "hash_map.h"

#include <pthread.h>

#include <algorithm>
#include <memory>

#include "redisimple/config.h"
#include "redisimple/log/error.h"
#include "redisimple/object/hash/hash_object.h"
#include "redisimple/object/redisimple_object.h"
#include "redisimple/util/random.h"

namespace redisimple::object::structure {

HashTable::HashTable(int size)
    : entry_num_(0), table_size_(size), size_mask_(size - 1) {
  table_.reset(new TableEntry[size]);
  memset(table_.get(), 0, size);
}

int HashTable::add_pair(unsigned int bucket,
                        std::unique_ptr<RedisimpleObject>& key,
                        std::unique_ptr<RedisimpleObject>& value) {
  if (table_[bucket].key_ == nullptr) {
    table_[bucket].key_.reset(key.release());
    table_[bucket].value_.reset(value.release());
    ++entry_num_;
    return 1;
  } else {
    TableEntry* ptr = &(table_[bucket]);
    // loop until ptr points to the matched entry or nullptr;
    while (ptr->next_) {
      if (key->compare(ptr->key_.get()) == 0) return 0;
      ptr = ptr->next_.get();
    }
    ptr->next_.reset(new TableEntry(key, value));
    ++entry_num_;
    return 1;
  }
}

int HashTable::replace_pair(unsigned int bucket,
                            std::unique_ptr<RedisimpleObject>& key,
                            std::unique_ptr<RedisimpleObject>& value) {
  if (table_[bucket].key_ == nullptr) {
    table_[bucket].key_.reset(key.release());
    table_[bucket].value_.reset(value.release());
    ++entry_num_;
    return 1;
  } else {
    TableEntry* ptr = &(table_[bucket]);
    // loop until ptr points to the matched entry or nullptr;
    while (ptr->next_) {
      if (key->compare(ptr->key_.get()) == 0) {
        ptr->value_.reset(value.release());
        return 0;
      }
      ptr = ptr->next_.get();
    }
    ptr->next_.reset(new TableEntry(key, value));
    ++entry_num_;
    return 1;
  }
}

RedisimpleObject* HashTable::get_value(unsigned int bucket,
                                       RedisimpleObject* key) {
  if (table_[bucket].key_ == nullptr) {
    return nullptr;
  } else {
    TableEntry* ptr = &(table_[bucket]);
    // loop until ptr points to the matched entry or nullptr;
    while (ptr->next_) {
      if (key->compare(ptr->key_.get()) == 0) {
        return ptr->value_.get();
      }
      ptr = ptr->next_.get();
    }
    return nullptr;
  }
}

int HashTable::delete_pair(unsigned int bucket, RedisimpleObject* key) {
  if (table_[bucket].key_->compare(key) == 0) {
    // delete the first entry
    if (table_[bucket].next_ != nullptr) {
      table_[bucket].key_.reset(table_[bucket].next_->key_.release());
      table_[bucket].value_.reset(table_[bucket].next_->value_.release());
      table_[bucket].next_.reset(table_[bucket].next_->next_.release());
    } else {
      table_[bucket].key_ = nullptr;
      table_[bucket].value_ = nullptr;
    }
    --entry_num_;
    return 1;
  }
  TableEntry* ptr = &(table_[bucket]);
  while (ptr->next_ != nullptr) {
    if (ptr->next_->key_->compare(key) == 0) {
      ptr->next_.reset(ptr->next_->next_.release());
      --entry_num_;
      return 1;
    }
    ptr = ptr->next_.get();
  }
  return 0;
}

void HashTable::add_without_check(int bucket,
                                  std::unique_ptr<TableEntry>& entry) {
  // if the bucket is empty, take the entry as the first one
  // otherwise, the entry will be added as the second entry in bucket
  TableEntry* head = &(table_[bucket]);
  if (head->key_ == nullptr) {
    head->key_.reset(entry->key_.release());
    head->value_.reset(entry->value_.release());
  } else {
    entry->next_.reset(head->next_.release());
    head->next_.reset(entry.release());
  }
}

HashMap::HashMap()
    : hash_table_(new HashTable(8)), expand_table_(), rehash_index_(-1) {}

HashMap::HashMap(int size)
    : hash_table_(new HashTable(size)), expand_table_(), rehash_index_(-1) {}

RST HashMap::structure_type() { return REDISIMPLE_STRUCTURE_HASH; }

// compare for hash object is not supported for hash map
int HashMap::compare(RedisimpleObject* ro) { return log::ERROR_NOT_DEFINED; }

// duplication is not supported for hash map
std::unique_ptr<RedisimpleObject> HashMap::duplicate() { return nullptr; }

int HashMap::size() { return hash_table_->entry_num_; }

// hash map has no hash value
int HashMap::hash() { return 0; }

// TODO: serialize of hash map
int HashMap::serialize(char* out_buf, int& offset, int& cnt) {
  return hash_table_->entry_num_;
}
// TODO: deserialize of hash map
int deserialize(char* argv[], int& offset, int& argc) { return 0; }

RedisimpleObject* HashMap::get(RedisimpleObject* key) {
  int hash_val = key->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashTable* target_table;
  if (rehash_index_ <= bucket) {
    target_table = hash_table_.get();
  } else {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  }
  auto result = target_table->get_value(bucket, key);
  return result;
}

// if the key is in dict, replace the value
// else add the pair to dict
int HashMap::set(std::unique_ptr<RedisimpleObject>& key,
                 std::unique_ptr<RedisimpleObject>& value) {
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

int HashMap::exist(RedisimpleObject* key) {
  int hash_val = key->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashTable* target_table;
  if (rehash_index_ <= bucket) {
    target_table = hash_table_.get();
  } else {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  }
  auto result = target_table->get_value(bucket, key);
  check_load_factor();
  return result == nullptr ? 0 : 1;
}

int HashMap::delete_pair(RedisimpleObject* key) {
  int hash_val = key->hash();
  int bucket = hash_val & hash_table_->size_mask_;
  HashTable* target_table;
  if (rehash_index_ <= bucket) {
    target_table = hash_table_.get();
  } else {
    target_table = expand_table_.get();
    bucket = hash_val & expand_table_->size_mask_;
  }
  int result = target_table->delete_pair(bucket, key);
  check_load_factor();
  return result;
}

int HashMap::add_pair(std::unique_ptr<RedisimpleObject>& key,
                      std::unique_ptr<RedisimpleObject>& value) {
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
  float load_factor = (float)hash_table_->entry_num_ / hash_table_->table_size_;
  // TODO: add conditions cosidering BGSAVE & BGREWRITEAOF
  if (load_factor > 2) {
    rehash_index_ = 0;
    expand_table_.reset(new HashTable(hash_table_->table_size_ << 1));
    rehash();
  } else if (load_factor < 0.1 && hash_table_->table_size_ > 8) {
    rehash_index_ = 0;
    expand_table_.reset(new HashTable(hash_table_->table_size_ >> 1));
    rehash();
  }
}

void HashMap::rehash() {
  int end_idx =
      std::min(Config::rehash_stride + rehash_index_, hash_table_->table_size_);
  for (; rehash_index_ < end_idx; ++rehash_index_) {
    // move entries from head to tail
    TableEntry& head = hash_table_->table_[rehash_index_];
    std::unique_ptr<TableEntry> p(head.next_.release());
    // add first entry to expand table
    int bucket = head.key_->hash() & expand_table_->size_mask_;
    hash_table_->add_pair(bucket, head.key_, head.value_);
    // add following entries to expand table
    while (p != nullptr) {
      std::unique_ptr<TableEntry> tmp(p->next_.release());
      bucket = p->key_->hash() & expand_table_->size_mask_;
      expand_table_->add_without_check(bucket, p);
      p.reset(tmp.release());
    }
  }
  if (rehash_index_ == hash_table_->table_size_) {
    // rehash finished
    hash_table_.reset(expand_table_.release());
    rehash_index_ = -1;
  }
}
}  // namespace redisimple::object::structure