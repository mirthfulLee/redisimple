#ifndef REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_
#define REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_
#include <memory>

namespace redisimple::object {
enum RedisimpleObjectType {
  REDISIMPLE_OBJECT_STRING = 0,
  REDISIMPLE_OBJECT_LIST,
  REDISIMPLE_OBJECT_HASH,
  REDISIMPLE_OBJECT_SET,
  REDISIMPLE_OBJECT_ZSET
};
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
typedef RedisimpleObjectType ROT;
typedef RedisimpleStructureType RST;
class RedisimpleObject {
 public:
  virtual ~RedisimpleObject() = 0;
  virtual ROT object_type() = 0;
  virtual RST structure_type() = 0;
  // only support compare for same object type;
  // but the concrete data structure could be different;
  virtual int compare(RedisimpleObject* ro) = 0;
  virtual std::unique_ptr<RedisimpleObject> duplicate() = 0;
  virtual int size() = 0;
  virtual int hash() = 0;
  virtual int serialize(char* out_buf, int& offset, int& cnt) = 0;
  virtual int deserialize(char* argv[], int& offset, int& argc) = 0;
};
}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_