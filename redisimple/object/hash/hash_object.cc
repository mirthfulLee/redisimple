#include "hash_object.h"

#include "redisimple/log/error.h"
#include "redisimple/object/hash/hash_map.h"
#include "redisimple/object/redisimple_object.h"

using redisimple::object::structure::HashMap;

namespace redisimple::object {
HashObject::HashObject() : data_(new HashMap) {}

ROT HashObject::object_type() { return REDISIMPLE_OBJECT_HASH; }
RST HashObject::structure_type() { return data_->structure_type(); }
int HashObject::compare(RedisimpleObject* ro) { return data_->compare(ro); }
std::unique_ptr<RedisimpleObject> HashObject::duplicate() {
  return data_->duplicate();
}
int HashObject::size() { return data_->size(); }
int HashObject::hash() { return data_->hash(); }
int HashObject::serialize(char* out_buf, int& offset, int& cnt) {
  return data_->serialize(out_buf, offset, cnt);
}
int HashObject::deserialize(char* argv[], int& offset, int& argc) {
  return data_->deserialize(argv, offset, argc);
}

RedisimpleObject* HashObject::get(RedisimpleObject* key) {
  return data_->get(key);
}
int HashObject::set(std::unique_ptr<RedisimpleObject>& key,
                    std::unique_ptr<RedisimpleObject>& value) {
  return data_->set(key, value);
}
int HashObject::exist(RedisimpleObject* key) { return data_->exist(key); }
int HashObject::delete_pair(RedisimpleObject* key) {
  return data_->delete_pair(key);
}
}  // namespace redisimple::object