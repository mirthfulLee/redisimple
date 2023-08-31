#ifndef REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_
#define REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_
#include <memory>

#include "structure/redisimple_data_structure.h"

namespace redisimple::object {
enum RedisimpleObjectType {
  REDISIMPLE_STRING = 0,
  REDISIMPLE_LIST,
  REDISIMPLE_HASH,
  REDISIMPLE_SET,
  REDISIMPLE_ZSET
};
enum RedisimpleObjectEncoding {
  REDISIMPLE_ENCODING_INT = 0,
  REDISIMPLE_ENCODING_EMBSTR,
  REDISIMPLE_ENCODING_RAW,
  REDISIMPLE_ENCODING_HT,
  REDISIMPLE_ENCODING_LINKEDLIST,
  REDISIMPLE_ENCODING_ZIPLIST,
  REDISIMPLE_ENCODING_INTSET,
  REDISIMPLE_ENCODING_SKIPLIST,
};
class RedisimpleObject {
 public:
  // 只占用4位
  // TODO: retrive type and encoding by interface
  RedisimpleObjectType type_ : 4;
  RedisimpleObjectEncoding encoding_ : 4;
  std::shared_ptr<redisimple::object::structure::RedisimpleDataStructure> data_ptr;
};
}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_