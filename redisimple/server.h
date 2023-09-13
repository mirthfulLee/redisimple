#ifndef REDISIMPLE_SERVER_H_
#define REDISIMPLE_SERVER_H_
#include <memory>

#include "data_base.h"
#include "redisimple/client.h"
namespace redisimple {
class Server {
 public:
  int load_config();

  int listen_init();
  // accept new connection(client);
  // deal with timer event;
  // process new request of client;
  // output result of process
  int start_main_loop();

 private:
  std::unique_ptr<DataBaseList> db_list_;
  std::unique_ptr<ClientList> client_list_;
  // TODO: event listener

 private:
  int new_client();
  int timer_event();
  int process_request(int fd);
  int output_result(int fd);
};
}  // namespace redisimple
#endif  // REDISIMPLE_SERVER_H_