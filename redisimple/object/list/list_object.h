#ifndef REDISIMPLE_OBJECT_SET_SET_OBJECT_H_
#define REDISIMPLE_OBJECT_SET_SET_OBJECT_H_
#include <memory>

#include "redisimple/object/redisimple_object.h"
namespace redisimple::object {
// Set includes multiple non-repeating member object
// could add, get random member, delete target member etc.
class ListObject : public RedisimpleObject {
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
  virtual int push_front(std::unique_ptr<RedisimpleObject>&);
  virtual int push_back(std::unique_ptr<RedisimpleObject>&);
  virtual int insert(int, std::unique_ptr<RedisimpleObject>&);
  virtual std::unique_ptr<RedisimpleObject> pop_front();
  virtual std::unique_ptr<RedisimpleObject> pop_back();
  virtual int remove(RedisimpleObject*);
  virtual int trim(int left, int right);
  virtual int set(int, std::unique_ptr<RedisimpleObject>&);
  virtual RedisimpleObject* at(int);

 private:
  std::unique_ptr<ListObject> data_;
};
}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_SET_SET_OBJECT_H_