#ifndef REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_
#define REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_
#include <memory>

#include "structure/redisimple_data_structure.h"
using redisimple::object::structure::RDS;
using redisimple::object::structure::RST;
namespace redisimple::object {
enum RedisimpleObjectType {
  REDISIMPLE_OBJECT_STRING = 0,
  REDISIMPLE_OBJECT_LIST,
  REDISIMPLE_OBJECT_HASH,
  REDISIMPLE_OBJECT_SET,
  REDISIMPLE_OBJECT_ZSET
};
typedef RedisimpleObjectType ROT;
class RedisimpleObject {
 public:
  virtual ~RedisimpleObject() = 0;
  virtual ROT object_type() = 0;
  virtual RST structure_type() { return data_->structure_type(); }
  // only support compare for same object type;
  // but the concrete data structure could be different;
  virtual int compare(RedisimpleObject* ro) {
    return data_->compare(ro->data_.get());
  }
  virtual std::unique_ptr<RedisimpleObject> duplicate() = 0;
  virtual int size() { return data_->size(); }
  virtual int hash() { return data_->hash(); }
  virtual char* serialize() = 0;
  virtual void deserialize() = 0;
  std::unique_ptr<RDS> data_;
};
}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_REDISIMPLE_OBJECT_H_