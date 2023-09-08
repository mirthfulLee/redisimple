#include "redisimple_string.h"

#include <cstring>
#include <memory>
#include <string>

#include "integer.h"
#include "redisimple/log/error.h"
#include "redisimple/object/redisimple_object.h"
#include "simple_dynamic_string.h"

using redisimple::object::structure::Integer;
using redisimple::object::structure::SimpleDynamicString;

namespace redisimple::object {
namespace {
// support integer [-9,999,999, 99,999,999];
const int k_sds_max_int_digits = 8;
const int min_integer = -9999999;
const int max_integer = 99999999;
}  // namespace

// the input str could not be nullptr;
// return the length of integer (str)
// return -1 if the str is too long or includes non-digit char
// so this function could not only get the length of integer str,
// but also check whether the str is an integer.
int length_of_integer(const char* str) {
  // if (str == nullptr) return -1;
  // special case: str == "-\0" should return -1
  int digit_cnt = *str == '-' && str[1] != '\0' ? 1 : 0;
  while (digit_cnt <= k_sds_max_int_digits && str[digit_cnt] != '\0') {
    if (str[digit_cnt] < '0' || str[digit_cnt] > '9') return -1;
    ++digit_cnt;
  }
  if (str[digit_cnt] != '\0') return -1;
  return digit_cnt;
}

String::String(const char* str) {
  int int_len = length_of_integer(str);
  if (int_len > 0) {
    data_.reset(new Integer(str));
  } else {
    data_.reset(new SimpleDynamicString(str));
  }
}

ROT String::object_type() { return REDISIMPLE_OBJECT_STRING; }

RST String::structure_type() { return data_->structure_type(); }

int String::compare(RedisimpleObject* ro) {
  if (ro->object_type() == REDISIMPLE_OBJECT_STRING)
    return log::ERROR_WRONG_TYPE;
  String* p = dynamic_cast<String*>(ro);
  if (data_->structure_type() == p->data_->structure_type())
    return data_->compare(p->data_.get());
  else if (data_->structure_type() == REDISIMPLE_STRUCTURE_INT)
    // only if caller is Integer can compare with different input;
    return data_->compare(p->data_.get());
  else
    return -p->data_->compare(data_.get());
}

std::unique_ptr<RedisimpleObject> String::duplicate() {
  return data_->duplicate();
}

int String::size() { return data_->size(); }

int String::hash() { return data_->hash(); }

int String::serialize(char* out_buf, int& offset, int& cnt) {
  return data_->serialize(out_buf, offset, cnt);
}

int String::deserialize(char* argv[], int& offset, int& argc) {
  return data_->deserialize(argv, offset, argc);
}

int String::set(const char* str) {
  int int_len = length_of_integer(str);
  if (int_len > 0) {
    if (data_->structure_type() == REDISIMPLE_STRUCTURE_INT)
      return data_->set(str);
    else
      data_.reset(new Integer(str));
  } else {
    if (data_->structure_type() == REDISIMPLE_STRUCTURE_RAW)
      return data_->set(str);
    else
      data_.reset(new SimpleDynamicString(str));
  }
  return 1;
}

int String::append(const char* str) {
  // convert Integer to SDS whatever
  if (data_->structure_type() != REDISIMPLE_STRUCTURE_RAW)
    itos(strlen(str) + k_sds_max_int_digits);
  return data_->append(str);
}

int String::increase_by(const char* str) { return data_->increase_by(str); }

int String::decrease_by(const char* str) { return data_->decrease_by(str); }

// make sure data_ is Integer and str_len > len(data_)
void String::itos(int str_len) {
  int len = 0, tmp;
  std::unique_ptr<char[]> buf(new char[str_len + 1]);
  data_->serialize(buf.get(), len, tmp);
  data_.reset(new SimpleDynamicString(buf, len, str_len - len));
}

}  // namespace redisimple::object