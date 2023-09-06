#ifndef REDISIMPLE_OBJECT_REDISIMPLE_STRING_H_
#define REDISIMPLE_OBJECT_REDISIMPLE_STRING_H_
#include <memory>

#include "redisimple_object.h"
#include "structure/redisimple_data_structure.h"
#include "structure/simple_dynamic_string.h"
namespace redisimple::object {
namespace {
int length_of_integer(const char *);
// transform cstring to int;
// support integer [-9,999,999, 99,999,999];
// return the digit of the integer (-1 if it is not a integer in range)
int string_to_integer(const char *, int &num);
}  // namespace
// the most commonly used object, like hash key, value...
// can be nested in other object
// its real data structure includes Integer & Simple Dynamic String
class String : public RedisimpleObject {
 public:
  String();
  String(const char *);
  ~String();
  int compare(RedisimpleObject *ro) { return data_->compare(ro->data_.get()); }
  int size() { return data_->size(); }
  int hash() { return data_->hash(); }
  char *serialize();
  void deserialize();

 private:
  void turn_to_sds();
};

}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_REDISIMPLE_STRING_H_