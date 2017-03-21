#ifndef _THREADSAFE_QUEUE_H
#define _THREADSAFE_QUEUE_H

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "page.h"

class ThreadsafeQueue {
public:
  // Constructor to initialize locks, conditional variables, and vector
  ThreadsafeQueue();
  void append(Page *page);
  Page *remove();
  bool isEmpty();

private:
  std::vector<Page *> queue;

  pthread_mutex_t queue_lock; // Lock
  pthread_cond_t queue_empty; // Condition indicating buffer is empty
};

#endif
