#ifndef REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_
#define REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_
#include <cstring>
#include <memory>

#include "redisimple/config.h"
#include "redisimple/object/redisimple_object.h"
#include "redisimple_string.h"
namespace redisimple::object::structure {

class SimpleDynamicString : public String {
 public:
  SimpleDynamicString();
  // build SDS by copying data from str
  SimpleDynamicString(const char* str);
  // copy constructor
  SimpleDynamicString(const SimpleDynamicString& sds);
  SimpleDynamicString(std::unique_ptr<char[]>& str, int len, int free);
  SimpleDynamicString(int num, int free = Config::large_sds_size);

 public:
  // function of RedisimpleObject
  RedisimpleStructureType structure_type();
  int compare(RedisimpleObject* sds);
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
  void copy(const char* str, int str_length);
  void catenate(const char* str, int str_length);
  void catenate(const SimpleDynamicString& sds);
  char* get();
  // less -> -1; equal -> 0; larger -> 1;
  // only keep the substr in range [left, right]
  // index < 0 is also supported
  void keep_in_range(int left, int right);
  // expand to target_length with '0'
  void grow_zero_to(int target_length);
  // remove all char in SDS that also in target
  void trim(const char* target);
  // remove first substr that match the pattern in SDS
  void remove(const char* pattern);

 private:
  // used bytes (not include the ending '\0')
  int len_;
  // free bytes (also not include '\0')
  int free_;
  std::unique_ptr<char[]> buf_;
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_