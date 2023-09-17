#include "simple_dynamic_string.h"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <stack>

#include "redisimple/log/error.h"
#include "redisimple/util/hash.h"
namespace redisimple::object::structure {

SimpleDynamicString::SimpleDynamicString() : len_(0), free_(0), buf_(nullptr){};

SimpleDynamicString::SimpleDynamicString(const char* str)
    : len_(0), free_(0), buf_(nullptr) {
  copy(str, strlen(str));
}

SimpleDynamicString::SimpleDynamicString(const char* str, int len)
    : len_(len), free_(0), buf_(new char[len + 1]) {
  copy(str, len);
}

SimpleDynamicString::SimpleDynamicString(const SimpleDynamicString& sds)
    : len_(0), free_(0), buf_(nullptr) {
  copy(sds.buf_.get(), sds.len_);
}
SimpleDynamicString::SimpleDynamicString(std::unique_ptr<char[]>& str, int len,
                                         int free)
    : len_(len), free_(free), buf_(str.release()) {}

SimpleDynamicString::SimpleDynamicString(int num, int free) : free_(free) {
  std::unique_ptr<char[]> buf_(new char[free + 1]);
  len_ = 0;
  if (num < 0) {
    buf_[0] = '-';
    ++len_;
  } else if (num == 0) {
    buf_[0] = '0';
    ++len_;
  }
  std::stack<int> stk;
  while (num) {
    stk.push(num % 10);
    num /= 10;
  }
  while (!stk.empty() && len_ < free) {
    buf_[len_] = '0' + stk.top();
    stk.pop();
    ++len_;
  }
  buf_[len_] = '\0';
  free -= len_;
}

RedisimpleStructureType SimpleDynamicString::structure_type() {
  return REDISIMPLE_STRUCTURE_RAW;
}

int SimpleDynamicString::compare(RedisimpleObject* sds) {
  if (sds->structure_type() != REDISIMPLE_STRUCTURE_RAW) return 1;
  return strcmp(buf_.get(),
                dynamic_cast<SimpleDynamicString*>(sds)->buf_.get());
}

std::unique_ptr<RedisimpleObject> SimpleDynamicString::duplicate() {
  return std::unique_ptr<RedisimpleObject>(new SimpleDynamicString(*this));
}

int SimpleDynamicString::size() { return len_; };

int SimpleDynamicString::hash() {
  return redisimple::util::murmurhash2(
      buf_.get(), len_, redisimple::Config::get_instance()->random_seed);
}

int SimpleDynamicString::serialize(char* out_buf, int& offset, int& cnt) {
  strcpy(out_buf + offset, buf_.get());
  offset += len_;
  ++cnt;
  return 0;
}

// take one
int SimpleDynamicString::deserialize(char* argv[], int& offset, int& argc) {
  copy(argv[offset], strlen(argv[offset]));
  ++offset;
  return 1;
}

int SimpleDynamicString::set(const char* str) {
  copy(str, strlen(str));
  return 1;
}
int SimpleDynamicString::append(const char* str) {
  catenate(str, strlen(str));
  return 1;
}
int increase_by(const char*) { return log::ERROR_NOT_DEFINED; }
int decrease_by(const char*) { return log::ERROR_NOT_DEFINED; }

void SimpleDynamicString::copy(const char* str, int str_length) {
  if (str_length == 0) {
    if (buf_) {
      buf_[0] = '\0';
      free_ += len_;
      len_ = 0;
    }
    return;
  }
  // have to reallocate memory
  if (free_ + len_ < str_length) {
    if (buf_) {
      // If sds is not empty, this means this sds is inconstant,
      // so predistribute more memory to buf_ for future operations.
      free_ = str_length < Config::large_sds_size ? str_length
                                                  : Config::more_free_space;
    } else {
      free_ = 0;
    }
    len_ = str_length;
    buf_.reset(new char[len_ + free_ + 1]);
  }
  strcpy(buf_.get(), str);
}

void SimpleDynamicString::catenate(const char* str, int str_length) {
  if (len_ == 0) {
    copy(str, str_length);
    return;
  }
  int old_len = len_;
  len_ += str_length;
  if (free_ < str_length) {
    // predistribute more memory to buf_ for future operations.
    free_ = len_ < Config::large_sds_size ? len_ : Config::more_free_space;
    std::unique_ptr<char[]> old_buf(buf_.release());
    buf_.reset(new char[len_ + free_ + 1]);
    strcpy(buf_.get(), old_buf.get());
  } else {
    free_ -= str_length;
  }
  strcpy(&buf_[old_len], str);
}

void SimpleDynamicString::catenate(const SimpleDynamicString& sds) {
  catenate(sds.buf_.get(), sds.len_);
}

void SimpleDynamicString::keep_in_range(int left, int right) {
  if (left < 0) left = len_ + left;
  if (right < 0) right = len_ + right;
  if (left > len_ || left > right) return;
  if (right > len_) right = len_;
  for (int i = 0; i <= right - left; ++i) buf_[i] = buf_[i + left];
  buf_[right + 1] = '\0';
  free_ += len_ - (right - left + 1);
  len_ = right - left + 1;
}

void SimpleDynamicString::grow_zero_to(int target_length) {
  if (!buf_) {
    len_ = target_length;
    free_ = 0;
    buf_.reset(new char[target_length + 1]);
    memset(buf_.get(), '0', target_length);
    buf_[target_length] = '\0';
    return;
  }
  if (target_length <= len_) return;
  if (target_length > len_ + free_) {
    std::unique_ptr<char[]> old_buf(buf_.release());
    free_ = target_length < Config::large_sds_size ? target_length
                                                   : Config::more_free_space;
    buf_.reset(new char[target_length + free_ + 1]);
    memcpy(buf_.get(), old_buf.get(), len_);
  } else {
    free_ = free_ + len_ - target_length;
  }
  memset(buf_.get() + len_, '0', target_length - len_);
  buf_[target_length] = '\0';
  len_ = target_length;
}

// the target for trim is short, so just compare char in buf with target's
void SimpleDynamicString::trim(const char* target) {
  // cnt char of buf_ that is not in target
  int cnt = 0, target_len = strlen(target);
  for (int i = 0; i < len_; ++i) {
    bool matched = false;
    for (int j = 0; j < target_len; ++j) {
      if (buf_[i] == target[j]) {
        matched = true;
        break;
      }
    }
    if (!matched) ++cnt;
    if (cnt != i) buf_[cnt] = buf_[i];
  }
  free_ += len_ - cnt;
  len_ = cnt;
  buf_[len_] = '\0';
}

// use kmp algorithm to match the pattern string
void SimpleDynamicString::remove(const char* pattern) {
  int pattern_len = strlen(pattern);
  if (!buf_ || len_ < pattern_len) return;
  int next[pattern_len];
  next[0] = 0;
  int front = 0, back = 1;
  // calculate next array
  while (back < pattern_len) {
    if (pattern[front] == pattern[back]) {
      ++front;
      next[back] = front;
      ++back;
    } else if (front == 0) {
      next[back] = 0;
      ++back;
    } else {
      front = next[front - 1];
    }
  }
  // try to match the substr from start_buf with pattern
  int start_buf = 0, i = 0;
  while (start_buf + pattern_len < len_) {
    if (buf_[start_buf + i] == pattern[i]) {
      ++i;
      if (i == pattern_len) {
        // remove the substr from start_buf
        len_ -= pattern_len;
        free_ += pattern_len;
        while (start_buf < len_) {
          buf_[start_buf] = buf_[start_buf + pattern_len];
        }
        buf_[len_] = '\0';
        return;
      }
    } else if (i == 0) {
      ++start_buf;
    } else {
      start_buf = start_buf + i - next[i];
      i = next[i];
    }
  }
}

}  // namespace redisimple::object::structure
