#include "redisimple_string.h"

#include <memory>

namespace redisimple::object {
int length_of_integer(const char* str) {
  if (str == nullptr) return -1;
  // special case: str == "-\0" should return -1
  int digit_cnt = *str == '-' && str[1] != '\0' ? 1 : 0;
  while (digit_cnt <= k_sds_max_int_digits && str[digit_cnt] != '\0') {
    if (str[digit_cnt] < '0' || str[digit_cnt] > '9') return -1;
    ++digit_cnt;
  }
  return digit_cnt;
}

int string_to_integer(const char* str, int& num) {
  int digit_cnt = length_of_integer(str);
  if (digit_cnt > 0 && digit_cnt <= k_sds_max_int_digits) {
    num = 0;
    for (int i = str[0] == '-' ? 1 : 0; i < digit_cnt; i++) {
      num = num * 10 + (str[i] - '0');
    }
    if (str[0] == '-') num *= -1;
  }
  return digit_cnt;
}

}  // namespace redisimple::object