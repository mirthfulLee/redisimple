#ifndef REDISIMPLE_OBJECT_HASH_HASH_MAP_H_
#define REDISIMPLE_OBJECT_HASH_HASH_MAP_H_
#include <cstring>
#include <memory>

#include "redisimple/object/hash/hash_object.h"
#include "redisimple/object/redisimple_object.h"

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
  std::unique_ptr<TableEntry[]> table_;
  // move entry to the new bucket (used during rehash)
  // this is faster than add;
  // make sure the next of `entry` is nullptr
  void add_without_check(int bucket, std::unique_ptr<TableEntry>& entry);
};

class HashMap : public HashObject {
 public:
  HashMap();
  HashMap(int size);

 public:
  RST structure_type();
  int compare(RedisimpleObject* ro);
  std::unique_ptr<RedisimpleObject> duplicate();
  int size();
  int hash();
  int serialize(char* out_buf, int& offset, int& cnt);
  int deserialize(char* argv[], int& offset, int& argc);

 public:
  RedisimpleObject* get(RedisimpleObject* key);
  int set(std::unique_ptr<RedisimpleObject>& key,
          std::unique_ptr<RedisimpleObject>& value);
  int exist(RedisimpleObject* key);
  int delete_pair(RedisimpleObject* key);

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
  TableEntry* get_random_pair();
  void clear();
  int add_pair(std::unique_ptr<RedisimpleObject>& key,
               std::unique_ptr<RedisimpleObject>& value);
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_HASH_HASH_MAP_H_