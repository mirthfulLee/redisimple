#ifndef REDISIMPLE_LOG_ERROR_H_
#define REDISIMPLE_LOG_ERROR_H_
namespace redisimple::log {
enum RedisimpleError {
  ERROR_NOT_EXIST = -1,     // can not find the value with the key
  ERROR_WRONG_TYPE = -2,    // the operation was adapted to wrong object
  ERROR_WRONG_SYNTAX = -3,  // the input syntax is not correct
  ERROR_PERMISSION = -4,    // the client have no permission to the DB
  ERROR_NOT_DEFINED = -5    // operation that is not defined
};
}

#endif