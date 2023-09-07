#ifndef REDISIMPLE_OBJECT_STRUCTURE_HASH_MAP_H_
#define REDISIMPLE_OBJECT_STRUCTURE_HASH_MAP_H_
#include <cstring>
#include <memory>

#include "redisimple/object/redisimple_object.h"
#include "redisimple_data_structure.h"
namespace redisimple::object::structure {
class TableEntry {
 public:
  TableEntry() : key_(nullptr), value_(nullptr), next_(nullptr) {}
  TableEntry(std::unique_ptr<RedisimpleObject>& key,
             std::unique_ptr<RedisimpleObject>& value);
  friend class HashTable;
  friend class HashMap;

 private:
  std::unique_ptr<RedisimpleObject> key_;
  std::unique_ptr<RedisimpleObject> value_;
  std::unique_ptr<TableEntry> next_;
};

class HashTable {
 public:
  HashTable(int size);
  int add_pair(unsigned int bucket, std::unique_ptr<RedisimpleObject>& key,
               std::unique_ptr<RedisimpleObject>& value);
  int replace_pair(unsigned int bucket, std::unique_ptr<RedisimpleObject>& key,
                   std::unique_ptr<RedisimpleObject>& value);
  TableEntry* get_random_pair();
  RedisimpleObject* get_value(unsigned int bucket, RedisimpleObject* key);
  int delete_pair(unsigned int bucket, RedisimpleObject* key);
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
                                    RedisimpleObject* target);
};

class HashMap : public RDS {
 public:
  HashMap();
  HashMap(int size);
  int add_pair(std::unique_ptr<RedisimpleObject>& key,
               std::unique_ptr<RedisimpleObject>& value);
  // if the key is in map, replace the value
  // else add the pair to map
  int replace_pair(std::unique_ptr<RedisimpleObject>& key,
                   std::unique_ptr<RedisimpleObject>& value);
  TableEntry* get_random_pair();
  RedisimpleObject* get_value(RedisimpleObject* key);
  int delete_pair(RedisimpleObject* key);
  void clear();
  // compare for HashMap is not allowed
  int compare(RDS*) { return 1; }
  RedisimpleStructureType structure_type() { return REDISIMPLE_STRUCTURE_HASH; }
  // duplicate is not supported for HashMap
  std::unique_ptr<RDS> duplicate() { return nullptr; }
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