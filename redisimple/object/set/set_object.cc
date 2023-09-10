#include "set_object.h"

#include <memory>

#include "redisimple/object/redisimple_object.h"
namespace redisimple::object {

// functions of Redisimple Object

ROT SetObject::object_type() { return REDISIMPLE_OBJECT_SET; }
RST SetObject::structure_type() { return data_->structure_type(); }
int SetObject::compare(RedisimpleObject* ro) { return data_->compare(ro); }
std::unique_ptr<RedisimpleObject> SetObject::duplicate() {
  return data_->duplicate();
}
int SetObject::size() { return data_->size(); }
int SetObject::hash() { return data_->hash(); }
int SetObject::serialize(char* out_buf, int& offset, int& cnt) {
  return data_->serialize(out_buf, offset, cnt);
}
int SetObject::deserialize(char* argv[], int& offset, int& argc) {
  return data_->deserialize(argv, offset, argc);
}

// functions of Set Object

int SetObject::add(std::unique_ptr<RedisimpleObject>& object) {
  return data_->add(object);
}
// return a random value in set;
RedisimpleObject* SetObject::random_value() { return data_->random_value(); }
// pop a random value and delete it
std::unique_ptr<RedisimpleObject> SetObject::pop_value() {
  return data_->pop_value();
}
int SetObject::exist(RedisimpleObject* target) { return data_->exist(target); }
int SetObject::delete_target(RedisimpleObject* target) {
  return data_->delete_target(target);
}

}  // namespace redisimple::object