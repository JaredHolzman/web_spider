#ifndef _THREADSAFE_QUEUE_H
#define _THREADSAFE_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <vector>
#include "page.h"

template <class T>
class ThreadsafeQueue {
 protected:
  std::vector<std::unique_ptr<T>> queue;
  std::mutex queue_mutex;  // Mutex
  std::condition_variable
      queue_empty_cv;  // Condition indicating buffer is empty
  bool finished;       // Boolean for when the queue is finished being used

 public:
  // Constructor to initialize locks, conditional variables, and vector
  ThreadsafeQueue();
  ~ThreadsafeQueue();
  virtual void append(std::unique_ptr<T> elem);
  virtual std::unique_ptr<T> remove();
  bool isEmpty();
  void setFinished();  // Mark the queue that no more data will be added
  void signal();       // Wake a thread waiting on the member condition variable
};

#endif
