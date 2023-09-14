#include "epoll_poller.h"

#include <sys/epoll.h>

#include "redisimple/config.h"
using redisimple::Config;
namespace redisimple::event {
EpollPoller::EpollPoller() {
  // init epoll table
  epfd_ = epoll_create(1024);
  // init event list
  events_.reset(new epoll_event[Config::event_list_size]);
}
int EpollPoller::add_file_event(int fd, int mask) {}
int EpollPoller::delete_file_event(int fd, int mask) {}
int EpollPoller::create_time_event() {}
// iterate to next pair of ready fd & mask;
// if all cached event have been processed,
// poll to get next group ready events, then this function is blocked
std::pair<int, int> EpollPoller::next_ready_event() {}
}  // namespace redisimple::event