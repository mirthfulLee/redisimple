#include "base_handler.h"

#include "redisimple/object/string/string_object.h"

namespace redisimple::handler {
// transparent namespace
namespace {}  // namespace
std::map<StringObject, handle_func> RequestHandler::request_handler_map_ = {
    {"SET", string_set},       {"GET", string_get},
    {"APPEND", string_append}, {"INCRBY", string_incrby},
    {"DECRBY", string_decrby}, {"STRLEN", string_strlen}};
int RequestHandler::execute_request(Client* client) {
  StringObject& opt = *dynamic_cast<StringObject*>(client->request_->at(0));
  handle_func opt_func = request_handler_map_[opt];
  return opt_func(client);
}
RequestHandler::RequestHandler() {}
RequestHandler* RequestHandler::load_handler() {
  if (handler_ == nullptr) {
    handler_.reset(new RequestHandler());
  }
  return handler_.get();
}
// TODO: implement concrete handle functions
}  // namespace redisimple::handler