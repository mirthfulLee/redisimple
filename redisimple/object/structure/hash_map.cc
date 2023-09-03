#include "hash_map.h"

#include <memory>

#include "redisimple_data_structure.h"

namespace redisimple::object::structure {
// innominate namespace for hash algorithm
namespace {
}  // namespace

HashTable::HashTable(int size)
    : entry_num_(0), table_size_(size), size_mask_(size - 1) {
  table_.reset(new std::unique_ptr<TableEntry>[size]);
  memset(table_.get(), 0, size);
}

unsigned int HashTable::get_hash_slot(RedisimpleDataStructure* key) {
  return key->hash() & size_mask_;
}

std::unique_ptr<TableEntry>& HashTable::get_last_pointer(
    unsigned int table_slot) {
  std::unique_ptr<TableEntry>* ptr = table_.get() + table_slot;
  if (*ptr == nullptr) {
    return *ptr;
  } else {
    // loop until ptr point to nullptr
    while (ptr->get()->next_) ptr = &(ptr->get()->next_);
    return ptr->get()->next_;
  }
}

std::unique_ptr<TableEntry>& HashTable::pointer_of_matched_entry(
    RedisimpleDataStructure* target) {
  unsigned int table_slot = get_hash_slot(target);
  std::unique_ptr<TableEntry>* ptr = table_.get() + table_slot;
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

void HashTable::add_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                         std::unique_ptr<RedisimpleDataStructure>& value) {
  unsigned int table_slot = get_hash_slot(key.get());
  get_last_pointer(table_slot).reset(new TableEntry(key, value));
  ++entry_num_;
}

void HashTable::replace_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                             std::unique_ptr<RedisimpleDataStructure>& value) {
  std::unique_ptr<TableEntry>& target_entry =
      pointer_of_matched_entry(key.get());
  if (target_entry == nullptr) {
    // target entry not exist, so add new entry
    target_entry.reset(new TableEntry(key, value));
    ++entry_num_;
  } else {
    // move the ownership to target entry
    target_entry->value_.reset(value.release());
  }
}

// TODO: how to get random number??
TableEntry* HashTable::get_random_pair() { return nullptr; }

int HashTable::delete_pair(std::unique_ptr<RedisimpleDataStructure>& key) {
  std::unique_ptr<TableEntry>& target_entry =
      pointer_of_matched_entry(key.get());
  if (target_entry) {
    if (target_entry->next_)
      target_entry.reset(target_entry->next_.release());
    else
      target_entry = nullptr;
    --entry_num_;
    return 1;
  }
  return 0;
}
HashMap::HashMap()
    : hash_table_(new HashTable(4)), expand_table_(), rehash_index_(-1) {}
HashMap::HashMap(int size)
    : hash_table_(new HashTable(size)), expand_table_(), rehash_index_(-1) {}
void HashMap::add_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                       std::unique_ptr<RedisimpleDataStructure>& value) {
  if (rehash_index_ >= 0)
    hash_table_->add_pair(key, value);
  else
    expand_table_->add_pair(key, value);
}
// if the key is in dict, replace the value
// else add the pair to dict
void HashMap::replace_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                           std::unique_ptr<RedisimpleDataStructure>& value) {}
TableEntry* get_random_pair();
int delete_pair(std::unique_ptr<RedisimpleDataStructure>& key);
void clear();
}  // namespace redisimple::object::structure