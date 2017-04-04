#ifndef _THREADSAFE_EXQUEUE_H
#define _THREADSAFE_EXQUEUE_H

#include "threadsafe_queue.h"
#include <unordered_set>

template <class T> class ThreadsafeExQueue : public ThreadsafeQueue<T> {
private:
  std::unordered_set<T> exclude_set;

public:
  // Constructor to initialize locks, conditional variables, and vector
  ThreadsafeExQueue();
  ~ThreadsafeExQueue();
  void append(T *elem);
  T *remove();
};

#endif
