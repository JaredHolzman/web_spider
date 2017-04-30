#ifndef _THREADSAFE_EXQUEUE_H
#define _THREADSAFE_EXQUEUE_H

#include <unordered_set>
#include "threadsafe_queue.h"

template <class T>
class ThreadsafeExQueue : public ThreadsafeQueue<T> {
 private:
  std::unordered_set<T> exclude_set;
  std::mutex set_mutex;  // Mutex

 public:
  // Constructor to initialize locks, conditional variables, and vector
  ThreadsafeExQueue();
  ~ThreadsafeExQueue();
  void append(std::unique_ptr<T> elem);
  std::unique_ptr<T> remove();
};

#endif
