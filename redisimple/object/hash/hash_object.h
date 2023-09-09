#ifndef REDISIMPLE_OBJECT_HASH_HASH_OBJECT_H_
#define REDISIMPLE_OBJECT_HASH_HASH_OBJECT_H_

#include <memory>

#include "redisimple/object/redisimple_object.h"
namespace redisimple::object {
class HashObject : public RedisimpleObject {
 public:
  HashObject();

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
  virtual RedisimpleObject* get(RedisimpleObject* key);
  virtual int set(std::unique_ptr<RedisimpleObject>& key,
                  std::unique_ptr<RedisimpleObject>& value);
  virtual int exist(RedisimpleObject* key);
  virtual int delete_pair(RedisimpleObject* key);

 private:
  std::unique_ptr<HashObject> data_;
};
}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_HASH_HASH_OBJECT_H_