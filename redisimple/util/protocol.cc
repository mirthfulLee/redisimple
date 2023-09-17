#include "protocol.h"

#include <stdio.h>

#include <memory>

#include "redisimple/object/hash/hash_object.h"
#include "redisimple/object/list/list_object.h"
#include "redisimple/object/redisimple_object.h"
#include "redisimple/object/string/integer.h"
#include "redisimple/object/string/string_object.h"
namespace redisimple::util {

// resolve string which is started with '+'
// the string is simple and does not contain '\r' or '\n'
std::unique_ptr<StringObject> resolve_string(char buffer[], int& offset) {
  int start = offset + 1;
  while (buffer[offset] != '\r' && buffer[offset + 1] != '\n') ++offset;
  buffer[offset] = '\0';
  offset += 2;
  return std::unique_ptr<StringObject>(new StringObject(buffer + start));
}

// resolve integer which is started with ':'
std::unique_ptr<StringObject> resolve_integer(char buffer[], int& offset) {
  int start = offset + 1, val;
  while (buffer[offset] != '\r' && buffer[offset + 1] != '\n') ++offset;
  offset += 2;
  sscanf(buffer + start, "%d", &val);
  return std::unique_ptr<StringObject>(new StringObject(val));
}

// resolve bulk string which is allowed to contain '\r' or '\n'
// the format is "${str_len}\r\n{str_content}\r\n"
std::unique_ptr<StringObject> resolve_bulk_string(char buffer[], int& offset) {
  int start = offset + 1;
  int len;
  sscanf(buffer + start, "*%d", &len);
  buffer[start + len] = '\0';
  offset += len + 3;
  return std::unique_ptr<StringObject>(new StringObject(buffer + start, len));
}

// resolve list object which is started with "*{list_size}\r\n"
std::unique_ptr<object::ListObject> resolve_list(char buffer[], int& offset) {
  int start = offset + 1;
  while (buffer[offset] != '\r' && buffer[offset + 1] != '\n') ++offset;
  buffer[offset] = '\0';
  offset += 2;
  int cnt;
  sscanf(buffer + start, "%d", &cnt);
  std::unique_ptr<object::ListObject> list(new object::ListObject());
  for (int i = 0; i < cnt; i++) {
    char type = buffer[offset];
    if (type == '+') {
      std::unique_ptr<RedisimpleObject> obj = resolve_string(buffer, offset);
      list->push_back(obj);
    } else if (type == ':') {
      std::unique_ptr<RedisimpleObject> obj = resolve_integer(buffer, offset);
      list->push_back(obj);
    } else if (type == '$') {
      std::unique_ptr<RedisimpleObject> obj =
          resolve_bulk_string(buffer, offset);
      list->push_back(obj);
    } else if (type == '*') {
      std::unique_ptr<RedisimpleObject> obj = resolve_list(buffer, offset);
      list->push_back(obj);
    }
  }
  return list;
}

}  // namespace redisimple::util