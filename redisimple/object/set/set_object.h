#ifndef REDISIMPLE_OBJECT_SET_SET_OBJECT_H_
#define REDISIMPLE_OBJECT_SET_SET_OBJECT_H_
#include <memory>

#include "redisimple/object/redisimple_object.h"
namespace redisimple::object {
// Set includes multiple non-repeating member object
// could add, get random member, delete target member etc.
class SetObject : public RedisimpleObject {
 public:
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

 public:
  virtual int add(RedisimpleObject*);
  // return a random value in set;
  virtual RedisimpleObject* random_value();
  // pop a random value and delete it
  virtual std::unique_ptr<RedisimpleObject> pop_value();
  virtual int exist(RedisimpleObject*);
  virtual int delete_target(RedisimpleObject*);

 private:
  std::unique_ptr<SetObject> data_;
};
}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_SET_SET_OBJECT_H_