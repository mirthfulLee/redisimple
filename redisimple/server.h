#ifndef REDISIMPLE_SERVER_H_
#define REDISIMPLE_SERVER_H_
#include <map>
#include <memory>

#include "data_base.h"
#include "redisimple/client.h"
#include "redisimple/event/poller.h"
namespace redisimple {
class Server {
 public:
  Server();

 public:
  int listen_init();
  // accept new connection(client);
  // deal with timer event;
  // process new request of client;
  // output result of process
  int start_main_loop();

 private:
  int listen_fd_;
  DataBaseList* db_list_;
  // map <fd, client>
  std::unique_ptr<ClientList> client_list_;
  // TODO: event poller
  event::Poller* poller_;

 private:
  int accept_new_client();
  int timer_event();
};
}  // namespace redisimple
#endif  // REDISIMPLE_SERVER_H_