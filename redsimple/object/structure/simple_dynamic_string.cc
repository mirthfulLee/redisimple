#include "simple_dynamic_string.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

namespace redisimple::object::structure {
namespace {
// TODO: read following number from config file
int large_sds_size = 1024;
int more_free_space = 1024;
}  // namespace

SimpleDynamicString::SimpleDynamicString() : len_(0), free_(0), buf_(nullptr){};

SimpleDynamicString::SimpleDynamicString(const char* str)
    : len_(0), free_(0), buf_(nullptr) {
  copy(str, strlen(str));
}
SimpleDynamicString::SimpleDynamicString(const SimpleDynamicString& sds)
    : len_(0), free_(0), buf_(nullptr) {
  copy(sds.buf_, sds.len_);
}
SimpleDynamicString::SimpleDynamicString(int length)
    : len_(0), free_(length), buf_(nullptr) {
  buf_ = new char[free_ + 1];
}
SimpleDynamicString::~SimpleDynamicString() { delete[] buf_; }
SimpleDynamicString* SimpleDynamicString::duplicate() {
  return new SimpleDynamicString(*this);
}
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
      delete[] buf_;
      free_ = str_length < large_sds_size ? str_length : more_free_space;
    } else {
      free_ = 0;
    }
    len_ = str_length;
    buf_ = new char[len_ + free_ + 1];
  }
  strcpy(buf_, str);
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
    free_ = len_ < large_sds_size ? len_ : more_free_space;
    char* old_buf = buf_;
    buf_ = new char[len_ + free_ + 1];
    strcpy(buf_, old_buf);
    delete[] old_buf;
  } else {
    free_ -= str_length;
  }
  strcpy(&buf_[old_len], str);
}
void SimpleDynamicString::catenate(const SimpleDynamicString& sds) {
  catenate(sds.buf_, sds.len_);
}

int SimpleDynamicString::compare(const SimpleDynamicString& sds) {
  return strcmp(buf_, sds.buf_);
}

void SimpleDynamicString::keep_in_range(int left, int right) {
  if (left > len_ || left < 0 || left >= right) return;
  if (right > len_ + 1) right = len_ + 1;
  for (int i = 0; i < right - left; ++i) buf_[i] = buf_[i + left];
  buf_[right] = '\0';
  len_ = right - left;
}

void SimpleDynamicString::grow_zero_to(int target_length) {
  if (!buf_) {
    len_ = target_length;
    free_ = 0;
    buf_ = new char[target_length + 1];
    memset(buf_, '0', target_length);
    buf_[target_length] = '\0';
    return;
  }
  if (target_length <= len_) return;
  if (target_length > len_ + free_) {
    char* old_buf = buf_;
    free_ = target_length < large_sds_size ? target_length : more_free_space;
    buf_ = new char[target_length + free_ + 1];
    memcpy(buf_, old_buf, len_);
    delete[] old_buf;
  } else {
    free_ = free_ + len_ - target_length;
  }
  memset(buf_ + len_, '0', target_length - len_);
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
