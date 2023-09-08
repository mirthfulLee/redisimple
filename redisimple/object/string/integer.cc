#include "integer.h"

#include <algorithm>
#include <cstdlib>
#include <memory>

#include "redisimple/config.h"
#include "redisimple/log/error.h"
#include "redisimple/object/redisimple_object.h"
#include "redisimple/util/hash.h"
#include "redisimple_string.h"
#include "simple_dynamic_string.h"

namespace redisimple::object::structure {

int stoi(const char* str) {
  int value = 0;
  for (int i = str[0] == '-' ? 1 : 0; i < str[i] != '\0'; i++) {
    value = value * 10 + (str[i] - '0');
  }
  if (str[0] == '-') value *= -1;
  return value;
}

int length_of_integer(int value) {
  int cnt = 0;
  if (value < 0) {
    ++cnt;
    value = -value;
  }
  while (value) {
    ++cnt;
    value /= 10;
  }
  return cnt;
}

Integer::Integer(const char* str) { value_ = stoi(str); }

RedisimpleStructureType Integer::structure_type() {
  return REDISIMPLE_STRUCTURE_INT;
}

int Integer::compare(RedisimpleObject* value) {
  if (value->structure_type() == REDISIMPLE_STRUCTURE_INT) {
    Integer* vptr = dynamic_cast<Integer*>(value);
    if (vptr->value_ == value_)
      return 0;
    else
      return vptr->value_ > value_ ? 1 : -1;
  } else {
    // build tmp string to compare.
    std::unique_ptr<SimpleDynamicString> tmp(new SimpleDynamicString(value_));
    return tmp->compare(value);
  }
}

std::unique_ptr<RedisimpleObject> Integer::duplicate() {
  return std::unique_ptr<Integer>(new Integer(value_));
}

int Integer::size() { return length_of_integer(value_); }

int Integer::hash() {
  return redisimple::util::murmurhash2(&value_, sizeof(value_),
                                       Config::random_seed);
}

int Integer::serialize(char* out_buf, int& offset, int& cnt) {
  int val = value_;
  if (val < 0) {
    out_buf[offset] = '-';
    val = -val;
    ++offset;
  }
  while (val) {
    out_buf[offset] = '0' + val % 10;
    ++offset;
    val /= 10;
  }
  return 0;
}

// TODO: complete deserialize
int Integer::deserialize(char* argv[], int& offset, int& argc) {
  return log::ERROR_NOT_DEFINED;
}

int Integer::set(const char* str) {
  value_ = stoi(str);
  return 0;
}

int Integer::append(const char*) { return redisimple::log::ERROR_NOT_DEFINED; }

int Integer::increase_by(const char* str) {
  value_ += stoi(str);
  return 0;
}

int Integer::decrease_by(const char* str) {
  value_ -= stoi(str);
  return 0;
}
}  // namespace redisimple::object::structure