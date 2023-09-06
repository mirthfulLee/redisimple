#include "redisimple_string.h"

#include <memory>
#include <stack>

namespace redisimple::object {
namespace {
const int k_sds_max_int_digits = 8;

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

std::unique_ptr<char[]> integer_to_string(int num, int& free) {
  std::unique_ptr<char[]> str(new char[free + 1]);
  int len = 0;
  if (num < 0) {
    str[0] = '-';
    ++len;
  } else if (num == 0) {
    str[0] = '0';
    ++len;
  }
  std::stack<int> stk;
  while (num) {
    stk.push(num % 10);
    num /= 10;
  }
  while (!stk.empty() && len < free) {
    str[len] = '0' + stk.top();
    stk.pop();
    ++len;
  }
  str[len] = '\0';
  free -= len;
  return str;
}
}  // namespace

}  // namespace redisimple::object