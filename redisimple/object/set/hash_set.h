#ifndef REDISIMPLE_OBJECT_HASH_HASH_MAP_H_
#define REDISIMPLE_OBJECT_HASH_HASH_MAP_H_
#include <cstring>
#include <memory>

#include "redisimple/object/hash/hash_object.h"
#include "redisimple/object/redisimple_object.h"
#include "redisimple/object/set/set_object.h"

namespace redisimple::object::structure {
class HashSetEntry {
 public:
  HashSetEntry();
  HashSetEntry(std::unique_ptr<RedisimpleObject>& object);
  friend class HashSetTable;
  friend class HashSet;

 private:
  std::unique_ptr<RedisimpleObject> object_;
  std::unique_ptr<HashSetEntry> next_;
};

class HashSetTable {
 public:
  HashSetTable(int size);

 public:
  int add(unsigned int bucket, std::unique_ptr<RedisimpleObject>& object);
  // return a random value in the bucket;
  RedisimpleObject* random_value(unsigned int bucket);
  // pop a random value in the bucket and `release` it
  std::unique_ptr<RedisimpleObject> pop_value(unsigned int bucket);
  int exist(unsigned int bucket, RedisimpleObject* object);
  int delete_target(unsigned int bucket, RedisimpleObject* object);
  friend class HashSet;

 private:
  unsigned int entry_num_;
  // table size = 2^n
  unsigned int table_size_;
  // always equals table_size - 1
  // since table size == 2^n;
  // size mask look like '000....0111111', which has n '1';
  unsigned int size_mask_;
  // point to an array of TableEntry pointer
  std::unique_ptr<HashSetEntry[]> table_;
  // return pointer to the entry whose key match the target;
  // if can not find target, return last pointer of slot.
  void add_without_check(int bucket, std::unique_ptr<HashSetEntry>&);
};

class HashSet : public SetObject {
 public:
  HashSet();

 public:
  RST structure_type();
  int compare(RedisimpleObject* ro);
  std::unique_ptr<RedisimpleObject> duplicate();
  int size();
  int hash();
  int serialize(char* out_buf, int& offset, int& cnt);
  int deserialize(char* argv[], int& offset, int& argc);

 public:
  int add(std::unique_ptr<RedisimpleObject>& object);
  // return a random value in set;
  RedisimpleObject* random_value();
  // pop a random value and delete it
  std::unique_ptr<RedisimpleObject> pop_value();
  int exist(RedisimpleObject* object);
  int delete_target(RedisimpleObject* object);

 private:
  std::unique_ptr<HashSetTable> hash_table_;
  std::unique_ptr<HashSetTable> expand_table_;
  // most of time, rehash_idx == -1;
  // during rehash process, rehash_idx move from 0 to n-1
  int rehash_index_;
  // check load factor of hash_table, decide whether start rehash
  void check_load_factor();
  // create a new thread to handle rehash
  void rehash();
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_HASH_HASH_MAP_H_