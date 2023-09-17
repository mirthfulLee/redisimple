#ifndef REDISIMPLE_UTIL_PROTOCOL_H_
#define REDISIMPLE_UTIL_PROTOCOL_H_
#include <memory>

#include "redisimple/client.h"
#include "redisimple/object/hash/hash_object.h"
#include "redisimple/object/list/list_object.h"
#include "redisimple/object/redisimple_object.h"
#include "redisimple/object/string/string_object.h"
namespace redisimple::util {
std::unique_ptr<object::StringObject> resolve_string(char buffer[],
                                                     int& offset);
std::unique_ptr<object::StringObject> resolve_integer(char buffer[],
                                                      int& offset);
std::unique_ptr<object::StringObject> resolve_bulk_string(char buffer[],
                                                          int& offset);
std::unique_ptr<object::ListObject> resolve_list(char buffer[], int& offset);
}  // namespace redisimple::util
#endif  // REDISIMPLE_UTIL_PROTOCOL_H_