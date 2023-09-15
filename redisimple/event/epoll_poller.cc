#include "epoll_poller.h"

#include <sys/epoll.h>

#include <utility>

#include "redisimple/config.h"
#include "redisimple/event/poller.h"
#include "redisimple/log/error.h"
using namespace redisimple::log;
namespace redisimple::event {
EpollPoller::EpollPoller() {
  event_limit_ = Config::event_list_size;
  // init epoll table
  epfd_ = epoll_create(1024);
  // init event list
  epoll_events_.reset(new epoll_event[event_limit_]);
  mask_.reset(new int[event_limit_]{0});
}

int EpollPoller::add_file_event(int fd, int mask) {
  epoll_event ee{0, 0};
  if (fd >= event_limit_) return ERROR_OUT_OF_BOUND;
  int op = mask_[fd] == EVENT_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;

  mask |= mask_[fd];
  ee.events = mask_to_epoll_event(mask);
  ee.data.fd = fd;

  if (epoll_ctl(epfd_, op, fd, &ee) == -1) return -1;
  return 0;
}

int EpollPoller::delete_file_event(int fd, int mask) {
  epoll_event ee{0, 0};
  int final_mask = mask_[fd] & (~mask);
  int op = final_mask == EVENT_NONE ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

  ee.events = mask_to_epoll_event(final_mask);
  ee.data.fd = fd;

  if (epoll_ctl(epfd_, op, fd, &ee) == -1) return -1;
  return 0;
}
int EpollPoller::create_time_event() {}
// iterate to next pair of ready fd & mask;
// if all cached event have been processed,
// poll to get next group ready events, then this function is blocked
std::pair<int, int> EpollPoller::next_ready_event() {
  if (processed_event_num_ == event_num_) {
    event_num_ = epoll_wait(epfd_, epoll_events_.get(), event_limit_, -1);
    processed_event_num_ = 0;
  }
  int fd = epoll_events_[processed_event_num_].data.fd;
  ++processed_event_num_;
  return std::pair<int, int>(fd, mask_[fd]);
}

int EpollPoller::epoll_event_to_mask(int event) {}
int mask_to_epoll_event(int mask) {
  // redisimple only has one process(thread), so oneshot is not necessary
  int event = 0;
  // TODO: use ET mode to improve performance
  if (mask & EVENT_READABLE) event |= EPOLLIN | EPOLLET;
  if (mask & EVENT_WRITABLE) event |= EPOLLOUT;
  if (mask & EVENT_CLOSE) event |= EPOLLRDHUP | EPOLLHUP | EPOLLERR;
  return event;
}
}  // namespace redisimple::event