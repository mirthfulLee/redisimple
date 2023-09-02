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
  REDISIMPLE_STRUCTURE_HT,
  REDISIMPLE_STRUCTURE_LINKEDLIST,
  REDISIMPLE_STRUCTURE_ZIPLIST,
  REDISIMPLE_STRUCTURE_INTSET,
  REDISIMPLE_STRUCTURE_SKIPLIST,
};
class RedisimpleDataStructure {
 public:
  virtual ~RedisimpleDataStructure() = 0;
  virtual int compare(RedisimpleDataStructure*);
  virtual RedisimpleStructureType structure_type();
  virtual std::unique_ptr<RedisimpleDataStructure> duplicate();
  virtual int size();
};
}
#endif // REDISIMPLE_OBJECT_STRUCTURE_REDISIMPLE_DATA_STRUCTURE_H_