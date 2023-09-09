#ifndef REDISIMPLE_OBJECT_STRING_INTEGER_H_
#define REDISIMPLE_OBJECT_STRING_INTEGER_H_

#include "redisimple/object/redisimple_object.h"
#include "string_object.h"
namespace redisimple::object::structure {
class Integer : public StringObject {
 public:
  Integer() : value_(0) {}
  Integer(int value) : value_(value) {}
  Integer(const char* str);

 public:
  // function of RedisimpleObject
  RedisimpleStructureType structure_type();
  int compare(RedisimpleObject*);
  std::unique_ptr<RedisimpleObject> duplicate();
  int size();
  int hash();
  int serialize(char* out_buf, int& offset, int& cnt);
  int deserialize(char* argv[], int& offset, int& argc);

 public:
  int set(const char*);
  int append(const char*);
  int increase_by(const char*);
  int decrease_by(const char*);

 private:
  int value_;
};
int stoi(const char* str);
int length_of_integer(int);
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRING_INTEGER_H_