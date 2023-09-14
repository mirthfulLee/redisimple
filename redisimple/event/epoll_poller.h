#ifndef REDISIMPLE_EVENT_EPOLL_POLLER_H_
#define REDISIMPLE_EVENT_EPOLL_POLLER_H_

#include <sys/epoll.h>

#include <memory>
#include <utility>

#include "poller.h"
namespace redisimple::event {
// poller is a kind of IO multiplexer
// after adding events to poller,
// user could simply get ready socket like iterator
class EpollPoller : public Poller {
 public:
  EpollPoller();

 public:
  // int add_listen_event(int fd);
  // add new socket to poller
  int create_file_event(int fd, int mask);
  // TODO: modify the defination of time event
  int create_time_event();
  // bind new event to fd
  int bind_event(int fd, int mask);
  // untie the event binded to fd
  int untie_event(int fd, int mask);
  // iterate to next pair of ready fd & mask;
  // if all cached event have been processed,
  // poll to get next group ready events, then this function is blocked
  std::pair<int, int> next_ready_event();

 private:
  int epfd_;
  std::unique_ptr<struct epoll_event[]> events_;
};
}  // namespace redisimple::event
#endif  // REDISIMPLE_EVENT_EPOLL_POLLER_H_