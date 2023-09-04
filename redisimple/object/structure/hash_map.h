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
  friend class HashMap;

 private:
  std::unique_ptr<RedisimpleDataStructure> key_;
  std::unique_ptr<RedisimpleDataStructure> value_;
  std::unique_ptr<TableEntry> next_;
};

class HashTable {
 public:
  HashTable(int size);
  int add_pair(unsigned int bucket,
               std::unique_ptr<RedisimpleDataStructure>& key,
               std::unique_ptr<RedisimpleDataStructure>& value);
  int replace_pair(unsigned int bucket,
                   std::unique_ptr<RedisimpleDataStructure>& key,
                   std::unique_ptr<RedisimpleDataStructure>& value);
  TableEntry* get_random_pair();
  RedisimpleDataStructure* get_value(unsigned int bucket,
                                     RedisimpleDataStructure* key);
  int delete_pair(unsigned int bucket, RedisimpleDataStructure* key);
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
  // return pointer to the entry whose key match the target;
  // if can not find target, return last pointer of slot.
  std::unique_ptr<TableEntry>& find(unsigned int bucket,
                                    RedisimpleDataStructure* target);
};

class HashMap : public RedisimpleDataStructure {
 public:
  HashMap();
  HashMap(int size);
  int add_pair(std::unique_ptr<RedisimpleDataStructure>& key,
               std::unique_ptr<RedisimpleDataStructure>& value);
  // if the key is in map, replace the value
  // else add the pair to map
  int replace_pair(std::unique_ptr<RedisimpleDataStructure>& key,
                   std::unique_ptr<RedisimpleDataStructure>& value);
  TableEntry* get_random_pair();
  RedisimpleDataStructure* get_value(RedisimpleDataStructure* key);
  int delete_pair(RedisimpleDataStructure* key);
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
  // most of time, rehash_idx == -1;
  // during rehash process, rehash_idx move from 0 to n-1
  int rehash_index_;
  // check load factor of hash_table, decide whether start rehash
  void check_load_factor();
  // create a new thread to handle rehash
  void rehash();
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_HASH_MAP_H_