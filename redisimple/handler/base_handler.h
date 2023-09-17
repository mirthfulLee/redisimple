#ifndef REDISIMPLE_HANDLER_BASE_HANDLER_H_
#define REDISIMPLE_HANDLER_BASE_HANDLER_H_
#include <memory>

#include "redisimple/client.h"
#include "redisimple/data_base.h"
#include "redisimple/object/redisimple_object.h"
namespace redisimple::handler {
// the interface of request execution
// take request and diliver it to corresponding concrete handler
class BaseHandler {
 public:
  std::unique_ptr<object::RedisimpleObject> execute_request(
      std::unique_ptr<object::RedisimpleObject>& request, Client* client);
  static BaseHandler* load_handler();

 private:
  BaseHandler();
  // instance of base handler that would
  static std::unique_ptr<BaseHandler> handler_;
  // handles requests that could change the stage of client;
  std::unique_ptr<BaseHandler> client_handler_;
  // handles requests that operates string objects
  std::unique_ptr<BaseHandler> string_handler_;
  // handles requests that operates list objects
  std::unique_ptr<BaseHandler> list_handler_;
  // handles requests that operates map objects
  std::unique_ptr<BaseHandler> map_handler_;
  // handles requests that operates set objects
  std::unique_ptr<BaseHandler> set_handler_;
};

}  // namespace redisimple::handler
#endif  // REDISIMPLE_HANDLER_BASE_HANDLER_H_