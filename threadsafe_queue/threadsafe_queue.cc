#include "threadsafe_queue.h"

ThreadsafeQueue::ThreadsafeQueue() : queue() {
  pthread_mutex_init(&queue_lock, NULL);
  pthread_cond_init(&queue_empty, NULL);
}

void ThreadsafeQueue::append(std::string *page_href) {
  pthread_mutex_lock(&queue_lock);
  queue.push_back(page_href);
  pthread_cond_signal(&queue_empty);
  pthread_mutex_unlock(&queue_lock);
}

std::string * ThreadsafeQueue::remove() {
  pthread_mutex_lock(&queue_lock);
  while (queue.empty()) {
    pthread_cond_wait(&queue_empty, &queue_lock);
  }

  std::string *head = queue.front();
  queue.erase(queue.begin());
  pthread_mutex_unlock(&queue_lock);

  return head;
}

bool ThreadsafeQueue::isEmpty() {
  return queue.empty();
}
