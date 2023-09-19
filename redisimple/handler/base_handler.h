#ifndef REDISIMPLE_HANDLER_BASE_HANDLER_H_
#define REDISIMPLE_HANDLER_BASE_HANDLER_H_
#include <map>
#include <memory>

#include "redisimple/client.h"
#include "redisimple/data_base.h"
#include "redisimple/object/list/list_object.h"
#include "redisimple/object/redisimple_object.h"
#include "redisimple/object/string/string_object.h"
namespace redisimple::handler {
typedef int (*handle_func)(Client* client);
// the interface of request execution
// take request and diliver it to corresponding concrete handler
class RequestHandler {
 public:
  virtual int execute_request(Client* client);
  static RequestHandler* load_handler();

 private:
  RequestHandler();
  static std::map<StringObject, handle_func> request_handler_map_;
  // instance of base handler
  static std::unique_ptr<RequestHandler> handler_;
  static int string_set(Client* client);
  static int string_get(Client* client);
  static int string_append(Client* client);
  static int string_incrby(Client* client);
  static int string_decrby(Client* client);
  static int string_strlen(Client* client);
};

}  // namespace redisimple::handler
#endif  // REDISIMPLE_HANDLER_BASE_HANDLER_H_