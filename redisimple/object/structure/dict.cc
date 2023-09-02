#include "dict.h"

#include <memory>

#include "redisimple_data_structure.h"

namespace redisimple::object::structure {
// innominate namespace for hash algorithm
namespace {
// MurmurHash2, by Austin Appleby
// Note - This code makes a few assumptions about how your machine behaves -
// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4
// And it has a few limitations -
// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.
// code from: https://sites.google.com/site/murmurhash/
unsigned int murmurhash(const void* key, int len, unsigned int seed) {
  // 'm' and 'r' are mixing constants generated offline.
  // They're not really 'magic', they just happen to work well.
  const unsigned int m = 0x5bd1e995;
  const int r = 24;
  // Initialize the hash to a 'random' value
  unsigned int h = seed ^ len;
  // Mix 4 bytes at a time into the hash
  const unsigned char* data = (const unsigned char*)key;
  while (len >= 4) {
    unsigned int k = *(unsigned int*)data;
    k *= m;
    k ^= k >> r;
    k *= m;
    h *= m;
    h ^= k;
    data += 4;
    len -= 4;
  }
  // Handle the last few bytes of the input array
  switch (len) {
    case 3:
      h ^= data[2] << 16;
    case 2:
      h ^= data[1] << 8;
    case 1:
      h ^= data[0];
      h *= m;
  };
  // Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.
  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;
  return h;
}

// set random seed
int random_seed = 0x27391764;
// set hash algorithm
unsigned int (*hash_function)(const void* key, int len,
                              unsigned int seed) = murmurhash;
}  // namespace

HashTable::HashTable(int size)
    : entry_num_(0), table_size_(size), size_mask_(size - 1) {
  table_.reset(new std::unique_ptr<TableEntry>[size]);
  memset(table_.get(), 0, size);
}

unsigned int HashTable::get_hash_slot(RedisimpleDataStructure* key) {
  return hash_function(key, key->size(), random_seed) & size_mask_;
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
TableEntry* HashTable::get_random_pair() {
  return nullptr;
}

int HashTable::delete_pair(std::unique_ptr<RedisimpleDataStructure>& key) {
  std::unique_ptr<TableEntry>& target_entry =
      pointer_of_matched_entry(key.get());
  if (target_entry) {
    if (target_entry->next_) target_entry.reset(target_entry->next_.release());
    else target_entry = nullptr;
    --entry_num_;
    return 1;
  }
  return 0;
}
}  // namespace redisimple::object::structure