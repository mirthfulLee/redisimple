#ifndef REDISIMPLE_OBJECT_STRING_OBJECT_H_
#define REDISIMPLE_OBJECT_STRING_OBJECT_H_
#include <memory>

#include "redisimple/object/redisimple_object.h"
namespace redisimple::object {
// the most commonly used object, like hash key, value...
// can be nested in other object
// its real data structure includes Integer & Simple Dynamic String
class StringObject : public RedisimpleObject {
 public:
  StringObject();
  StringObject(const char*);

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
  virtual int set(const char*);
  virtual int append(const char*);
  virtual int increase_by(const char*);
  virtual int decrease_by(const char*);

 private:
  std::unique_ptr<StringObject> data_;
  void itos(int str_len);
};

int length_of_integer(const char*);
std::unique_ptr<StringObject> create_string(const char*);
}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_STRING_OBJECT_H_