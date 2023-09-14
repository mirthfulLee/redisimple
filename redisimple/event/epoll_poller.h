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
  int add_file_event(int fd, int mask);
  int delete_file_event(int fd, int mask);
  // TODO: modify the defination of time event
  int create_time_event();
  // iterate to next pair of ready fd & mask;
  // if all cached event have been processed,
  // poll to get next group ready events, then this function is blocked
  std::pair<int, int> next_ready_event();

 private:
  int epfd_;
  std::unique_ptr<struct epoll_event[]> epoll_events_;
  // the total number of ready events
  int event_num_;
  int processed_event_num_;

 private:
  int epoll_event_to_mask(int event);
  int mask_to_epoll_event(int mask);
};
}  // namespace redisimple::event
#endif  // REDISIMPLE_EVENT_EPOLL_POLLER_H_