#ifndef REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_
#define REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_
#include <memory>
#include <cstring>
namespace redisimple::object::structure {

class SimpleDynamicString {
 public:
  SimpleDynamicString();
  SimpleDynamicString(const char* str);
  // 拷贝构造函数
  SimpleDynamicString(const SimpleDynamicString &sds);
  SimpleDynamicString(int length);
  ~SimpleDynamicString();
  int length() { return len_; };
  int avail() { return free_; };
  void clear();
  SimpleDynamicString* duplicate();
  void copy(const char* str, int str_length);
  void catenate(const char* str, int str_length);
  void catenate(const SimpleDynamicString& sds);
  // less -> -1; equal -> 0; larger -> 1;
  int compare(const SimpleDynamicString& sds);
  // only keep the substr in range [left, right)
  void keep_in_range(int left, int right);
  // expand to target_length with '0'
  void grow_zero_to(int target_length);
  // remove target substr in SDS
  void trim(const char* str);

 private:
  // used bytes (not include the ending '\0')
  int len_;
  // free bytes (also not include '\0')
  int free_;
  char* buf_;
};
}  // namespace redisimple::object::structure
#endif  // REDISIMPLE_OBJECT_STRUCTURE_SIMPLE_DYNAMIC_STRING_H_