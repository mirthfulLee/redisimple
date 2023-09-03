#ifndef REDISIMPLE_OBJECT_STRUCTURE_HASH_MAP_H_
#define REDISIMPLE_OBJECT_STRUCTURE_HASH_MAP_H_
#include <cstddef>
#include <cstring>
#include <memory>

#include "redisimple_data_structure.h"
namespace redisimple::object::structure {
class TableEntry {
 public:
  TableEntry() : key_(nullptr), value_(nullptr), next_(nullptr) {}
  TableEntry(std::unique_ptr<RedisimpleDataStructure>& key,
             std::unique_ptr<RedisimpleDataStructure>& value);
  friend class HashTable;

 private:
  std::unique_ptr<RedisimpleDataStructure> key_;
  std::unique_ptr<RedisimpleDataStructure> value_;
  std::unique_ptr<TableEntry> next_;
};

class HashTable {
 public:
  HashTable(int size);
  void add_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                std::unique_ptr<RedisimpleDataStructure>& value);
  void replace_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                    std::unique_ptr<RedisimpleDataStructure>& value);
  TableEntry* get_random_pair();
  int delete_pair(std::unique_ptr<RedisimpleDataStructure>& key);
  friend class HashMap;

 private:
  unsigned int entry_num_;
  // table size = 2^n
  unsigned int table_size_;
  // always equals table_size - 1
  // since table size == 2^n;
  // size mask look like '000....0111111', which has n '1';
  unsigned int size_mask_;
  // point to an array of TableEntry pointer
  std::unique_ptr<std::unique_ptr<TableEntry>[]> table_;
  unsigned int get_hash_slot(RedisimpleDataStructure* key);
  // get the last pointer of specific slot whose value is nullptr now;
  std::unique_ptr<TableEntry>& get_last_pointer(unsigned int table_slot);
  // return pointer to the entry whose key match the target;
  // if can not find target, return last pointer of slot.
  std::unique_ptr<TableEntry>& pointer_of_matched_entry(
      RedisimpleDataStructure* target);
};

class HashMap : public RedisimpleDataStructure {
 public:
  HashMap();
  HashMap(int size);
  void add_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                std::unique_ptr<RedisimpleDataStructure>& value);
  // if the key is in map, replace the value
  // else add the pair to map
  void replace_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                    std::unique_ptr<RedisimpleDataStructure>& value);
  TableEntry* get_random_pair();
  int delete_pair(std::unique_ptr<RedisimpleDataStructure>& key);
  void clear();
  // compare for HashMap is not allowed
  int compare(RedisimpleDataStructure*) { return 1; }
  RedisimpleStructureType structure_type() { return REDISIMPLE_STRUCTURE_HASH; }
  // duplicate is not supported for HashMap
  std::unique_ptr<RedisimpleDataStructure> duplicate() { return nullptr; }
  // hash is not support for HashMap
  int hash() { return 0; }

 private:
  std::unique_ptr<HashTable> hash_table_;
  std::unique_ptr<HashTable> expand_table_;
  int rehash_index_;
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_HASH_MAP_H_