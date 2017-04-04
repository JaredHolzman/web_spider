#ifndef _THREADSAFE_QUEUE_H
#define _THREADSAFE_QUEUE_H

#include "page.h"
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

template <class T> class ThreadsafeQueue {
protected:
  std::vector<T *> queue;
  std::mutex queue_mutex; // Mutex
  std::condition_variable
      queue_empty_cv; // Condition indicating buffer is empty
  bool finished;      // Boolean for when the queue is finished being used

public:
  // Constructor to initialize locks, conditional variables, and vector
  ThreadsafeQueue();
  ~ThreadsafeQueue();
  virtual void append(T *elem);
  virtual T *remove();
  bool isEmpty();
  void setFinished(); // Mark the queue that no more data will be added
  void signal();      // Wake a thread waiting on the member condition variable
};

#endif
