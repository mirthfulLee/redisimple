#ifndef REDISIMPLE_SERVER_H_
#define REDISIMPLE_SERVER_H_
#include <memory>

#include "data_base.h"
namespace redisimple {
class Server {
 public:
 private:
  std::unique_ptr<DataBaseList> dbs_;
};
}  // namespace redisimple
#endif  // REDISIMPLE_SERVER_H_