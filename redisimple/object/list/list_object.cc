#include "list_object.h"
namespace redisimple::object {

// functions of Redisimple Object

ROT ListObject::object_type() { return REDISIMPLE_OBJECT_LIST; }
RST ListObject::structure_type() { return data_->structure_type(); }
int ListObject::compare(RedisimpleObject* ro) { return data_->compare(ro); }
std::unique_ptr<RedisimpleObject> ListObject::duplicate() {
  return data_->duplicate();
}
int ListObject::size() { return data_->size(); }
int ListObject::hash() { return data_->hash(); }
int ListObject::serialize(char* out_buf, int& offList, int& cnt) {
  return data_->serialize(out_buf, offList, cnt);
}
int ListObject::deserialize(char* argv[], int& offList, int& argc) {
  return data_->deserialize(argv, offList, argc);
}

// functions of List Object;

int ListObject::push_front(std::unique_ptr<RedisimpleObject>& object) {
  return data_->push_front(object);
}
int ListObject::push_back(std::unique_ptr<RedisimpleObject>& object) {
  return data_->push_back(object);
}
int ListObject::insert(int index, std::unique_ptr<RedisimpleObject>& object) {
  return data_->insert(index, object);
}
std::unique_ptr<RedisimpleObject> ListObject::pop_front() {
  return data_->pop_front();
}
std::unique_ptr<RedisimpleObject> ListObject::pop_back() {
  return data_->pop_back();
}
int ListObject::remove(RedisimpleObject* target) {
  return data_->remove(target);
}
int ListObject::trim(int left, int right) { return data_->trim(left, right); }
int ListObject::set(int index, std::unique_ptr<RedisimpleObject>& object) {
  return data_->set(index, object);
}
RedisimpleObject* ListObject::at(int index) { return data_->at(index); }
}  // namespace redisimple::object