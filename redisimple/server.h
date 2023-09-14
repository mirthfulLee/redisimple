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
  int load_config();

  int listen_init();
  // accept new connection(client);
  // deal with timer event;
  // process new request of client;
  // output result of process
  int start_main_loop();

 private:
  int listen_fd_;
  std::unique_ptr<DataBaseList> db_list_;
  // map <fd, client>
  std::map<int, Client> clients_;
  // TODO: event poller
  std::unique_ptr<event::Poller> poller_;

 private:
  int new_client();
  int delete_client();
  int timer_event();
  int process_request(int fd);
  int output_result(int fd);
};
}  // namespace redisimple
#endif  // REDISIMPLE_SERVER_H_