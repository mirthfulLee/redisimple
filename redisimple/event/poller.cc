#include "poller.h"

#include <memory>

#include "redisimple/event/epoll_poller.h"

namespace redisimple::event {
std::unique_ptr<Poller> Poller::poller_instance_;
Poller* Poller::get_poller_instance() {
  // init poller with macro which depend on the platform
  if (poller_instance_ == nullptr) {
#if EPOLL_POLLER
    poller_instance_.reset(new EpollPoller);
#endif
  }

  return poller_instance_.get();
}
}  // namespace redisimple::event