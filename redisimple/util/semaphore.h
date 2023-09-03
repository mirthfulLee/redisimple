#ifndef REDISIMPLE_UTIL_SEMAPHORE_H_
#define REDISIMPLE_UTIL_SEMAPHORE_H_

#include <pthread.h>
#include <semaphore.h>

#include <exception>
namespace redisimple::util {
class Locker {
 public:
  Locker() {
    if (pthread_mutex_init(&mutex_, NULL) != 0) throw std::exception();
  }
  ~Locker() { pthread_mutex_destroy(&mutex_); }
  int lock() { return pthread_mutex_lock(&mutex_); }
  int unlock() { return pthread_mutex_unlock(&mutex_); }

 private:
  pthread_mutex_t mutex_;
};
class Semaphore {
 public:
  Semaphore() {
    if (sem_init(&sem_, 0, 0) != 0) throw std::exception();
  }
  Semaphore(int num) {
    if (sem_init(&sem_, 0, num) != 0) throw std::exception();
  }
  ~Semaphore() { sem_destroy(&sem_); }
  int wait() { return sem_wait(&sem_); }
  int post() { return sem_post(&sem_); }

 private:
  sem_t sem_;
};
}  // namespace redisimple::util
#endif