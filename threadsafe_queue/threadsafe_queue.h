#ifndef _THREADSAFE_QUEUE_H
#define _THREADSAFE_QUEUE_H

#include "page.h"
#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

class ThreadsafeQueue {
public:
  // Constructor to initialize locks, conditional variables, and vector
  ThreadsafeQueue();
  void append(Page *page);
  Page *remove();
  bool isEmpty();
  void setFinished(); // Mark the queue that no more data will be added
  void signal();      // Wake a thread waiting on the member condition variable

private:
  std::vector<Page *> queue;

  pthread_mutex_t queue_lock; // Lock
  pthread_cond_t queue_empty; // Condition indicating buffer is empty
  bool finished; // Boolean for when the queue is finished being used
};

#endif
