#ifndef REDISIMPLE_OBJECT_SET_SET_OBJECT_H_
#define REDISIMPLE_OBJECT_SET_SET_OBJECT_H_
#include <memory>

#include "redisimple/object/redisimple_object.h"
namespace redisimple::object {
// Set includes multiple non-repeating member object
// could add, get random member, delete target member etc.
class SetObject : public RedisimpleObject {
 public:
  ROT object_type();
  virtual RST structure_type();
  virtual int compare(RedisimpleObject* ro);
  virtual std::unique_ptr<RedisimpleObject> duplicate();
  virtual int size();
  virtual int hash();
  virtual int serialize(char* out_buf, int& offset, int& cnt);
  virtual int deserialize(char* argv[], int& offset, int& argc);

 public:
  virtual int add(std::unique_ptr<RedisimpleObject>& object);
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