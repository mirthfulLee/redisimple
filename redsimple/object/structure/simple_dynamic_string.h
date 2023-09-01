#ifndef REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_
#define REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_
#include <cstring>
#include <memory>

#include "redisimple_data_structure.h"
namespace redisimple::object::structure {

class SimpleDynamicString : public RedisimpleDataStructure {
 public:
  SimpleDynamicString();
  // build SDS by copying data from str
  SimpleDynamicString(const char* str);
  // copy constructor
  SimpleDynamicString(const SimpleDynamicString& sds);
  SimpleDynamicString(int length);
  ~SimpleDynamicString();
  int length() { return len_; };
  int avail() { return free_; };
  void clear();
  std::unique_ptr<RedisimpleDataStructure> duplicate();
  void copy(const char* str, int str_length);
  void catenate(const char* str, int str_length);
  void catenate(const SimpleDynamicString& sds);
  // less -> -1; equal -> 0; larger -> 1;
  int compare(const SimpleDynamicString& sds);
  // only keep the substr in range [left, right)
  void keep_in_range(int left, int right);
  // expand to target_length with '0'
  void grow_zero_to(int target_length);
  // remove all char in SDS that also in target
  void trim(const char* target);
  // remove first substr that match the pattern in SDS
  void remove(const char* pattern);
  RedisimpleStructureType structure_type() { return REDISIMPLE_STRUCTURE_RAW; }

 private:
  // used bytes (not include the ending '\0')
  int len_;
  // free bytes (also not include '\0')
  int free_;
  char* buf_;
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_