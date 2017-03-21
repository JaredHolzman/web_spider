#ifndef _THREADSAFE_QUEUE_H
#define _THREADSAFE_QUEUE_H

#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

class ThreadsafeQueue {
public:
  // Constructor to initialize locks, conditional variables, and vector
  ThreadsafeQueue();
  void append(std::string *page_href);
  std::string *remove();
  void clear();
  int size();
  bool isEmpty();

private:
  std::vector<std::string *> queue;

  pthread_mutex_t queue_lock; // Lock
  pthread_cond_t queue_empty; // Condition indicating buffer is empty
};

#endif
