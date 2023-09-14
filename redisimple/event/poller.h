#ifndef REDISIMPLE_EVENT_POLLER_H_
#define REDISIMPLE_EVENT_POLLER_H_

#include <utility>
namespace redisimple::event {

enum REDISIMPLE_EVENT {
  EVENT_NONE = 0,
  EVENT_READABLE = 1 << 1,
  EVENT_WRITABLE = 1 << 2,
  EVENT_CLOSE = 1 << 3,
};
// poller is a kind of IO multiplexer
// after adding events to poller,
// user could simply get ready socket like iterator
class Poller {
 public:
  Poller();
  virtual ~Poller() = 0;

 public:
  virtual int add_file_event(int fd, int mask) = 0;
  virtual int delete_file_event(int fd, int mask) = 0;
  // TODO: modify the defination of time event
  virtual int create_time_event() = 0;
  // iterate to next pair of ready fd & mask;
  // if all cached event have been processed,
  // poll to get next group ready events, then this function is blocked
  virtual std::pair<int, int> next_ready_event() = 0;
};
}  // namespace redisimple::event
#endif  // REDISIMPLE_EVENT_POLLER_H_