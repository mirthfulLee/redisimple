#include "simple_dynamic_string.h"

#include <cctype>
#include <cstdlib>
#include <cstring>

namespace redisimple::object::structure {
namespace {
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
  
}

void trim(const char* str);

void transform_to_string();

}  // namespace redisimple::object::structure
