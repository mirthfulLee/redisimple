#ifndef REDISIMPLE_OBJECT_REDISIMPLE_STRING_H_
#define REDISIMPLE_OBJECT_REDISIMPLE_STRING_H_
#include <memory>

#include "structure/redisimple_data_structure.h"

namespace redisimple::object {
const int k_sds_max_int_digits = 8;
  int length_of_integer(const char*);
  // transform cstring to int;
  // support integer [-9,999,999, 99,999,999];
  // return the digit of the integer (-1 if it is not a integer in range)
  int string_to_integer(const char*, int &num);

}  // namespace redisimple::object
#endif  // REDISIMPLE_OBJECT_REDISIMPLE_STRING_H_