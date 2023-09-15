#include "server.h"

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

#include <memory>
#include <utility>

#include "config.h"
#include "event/epoll_poller.h"
#include "redisimple/client.h"
#include "redisimple/event/poller.h"

namespace redisimple {
Server::Server() : db_list_(nullptr), client_list_(nullptr) {
  poller_->get_poller_instance();
}

int Server::listen_init() {
  int port = Config::redisimple_port;
  listen_fd_ = socket(PF_INET, SOCK_STREAM, 0);

  sockaddr_in addr;
  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  int flag = 1;
  setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));
  int retval = bind(listen_fd_, (sockaddr*)&addr, sizeof(addr));
  if (retval < 0) return retval;
  retval = listen(listen_fd_, 16);
  if (retval < 0) return retval;

  poller_->add_file_event(listen_fd_, event::EVENT_READABLE);

  return 0;
}
// accept new connection(client);
// deal with timer event;
// process new request of client;
// output result of process
int Server::start_main_loop() {
  int stop = 0;
  while (!stop) {
    std::pair<int, int> event = poller_->next_ready_event();
    if (event.first == listen_fd_) {
      // listen socket get new conncetion request
      accept_new_client();
    } else if (event.second & event::EVENT_READABLE) {
      Client* client = client_list_->get_client_with_fd(event.first);
      client->read_and_execute(event.second);
    } else if (event.second & event::EVENT_WRITABLE) {
      Client* client = client_list_->get_client_with_fd(event.first);
      client->write();
    } else if (event.second & event::EVENT_CLOSE) {
      client_list_->delete_client_with_fd(event.first);
    }
  }
  return 0;
}

int Server::accept_new_client() {
  sockaddr_in client_addr;
  socklen_t addr_len = sizeof(client_addr);
  int fd = accept(listen_fd_, (sockaddr*)&client_addr, &addr_len);
  if (fd <= 0) return -1;
  std::unique_ptr<Client> client(new Client(fd, 0));
  client_list_->add_client(client);
  return 1;
}

int Server::timer_event() {
  // TODO: define timer event
  return -1;
}
}  // namespace redisimple