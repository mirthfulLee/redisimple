#ifndef REDISIMPLE_OBJECT_STRUCTURE_REDISIMPLE_DATA_STRUCTURE_H_
#define REDISIMPLE_OBJECT_STRUCTURE_REDISIMPLE_DATA_STRUCTURE_H_
#include <memory>

namespace redisimple::object::structure {
enum RedisimpleStructureType {
  REDISIMPLE_STRUCTURE_INT = 0,
  // Simple Dynamic String with embstr encoding
  REDISIMPLE_STRUCTURE_EMBSTR,
  // Simple Dynamic String
  REDISIMPLE_STRUCTURE_RAW,
  // hash table
  REDISIMPLE_STRUCTURE_HASH,
  REDISIMPLE_STRUCTURE_LINKEDLIST,
  REDISIMPLE_STRUCTURE_ZIPLIST,
  REDISIMPLE_STRUCTURE_INTSET,
  REDISIMPLE_STRUCTURE_SKIPLIST,
};
typedef RedisimpleStructureType RST;

class RedisimpleDataStructure {
 public:
  virtual ~RedisimpleDataStructure() = 0;
  // only support compare for same data structure;
  virtual int compare(RedisimpleDataStructure*) = 0;
  virtual RST structure_type() = 0;
  virtual std::unique_ptr<RedisimpleDataStructure> duplicate() = 0;
  virtual int size() = 0;
  virtual int hash() = 0;
};
typedef RedisimpleDataStructure RDS;
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_REDISIMPLE_DATA_STRUCTURE_H_