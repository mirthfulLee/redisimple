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
class ReaderWriterLocker {
 public:
  ReaderWriterLocker() : write_lock(), reader_cnt_lock(), reader_cnt(0) {}
  int read() {
    reader_cnt_lock.lock();
    ++reader_cnt;
    if (reader_cnt == 1) write_lock.lock();
    reader_cnt_lock.unlock();
  }
  int done_read() {
    reader_cnt_lock.lock();
    --reader_cnt;
    if (reader_cnt == 0) write_lock.unlock();
    reader_cnt_lock.unlock();
  }
  int write() { write_lock.lock(); }
  int done_write() { write_lock.unlock(); }

 private:
  Locker write_lock, reader_cnt_lock;
  int reader_cnt;
};
}  // namespace redisimple::util
#endif