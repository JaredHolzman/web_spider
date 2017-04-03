#include "threadsafe_queue.h"

ThreadsafeQueue::ThreadsafeQueue()
    : queue(), queue_mutex(), queue_empty_cv(), finished(false) {}
ThreadsafeQueue::~ThreadsafeQueue(){}

void ThreadsafeQueue::append(Page *page_href) {
  std::unique_lock<std::mutex> lock(queue_mutex);
  queue.push_back(page_href);
  lock.unlock();
  queue_empty_cv.notify_one();
}

Page *ThreadsafeQueue::remove() {
  std::unique_lock<std::mutex> lock(queue_mutex);
  while (queue.empty()) {
    if (finished) {
      lock.unlock();
      return NULL;
    }
    queue_empty_cv.wait(lock);
  }

  Page *head = queue.front();
  queue.erase(queue.begin());
  lock.unlock();

  return head;
}

bool ThreadsafeQueue::isEmpty() { return queue.empty(); }

void ThreadsafeQueue::setFinished() { finished = true; };

void ThreadsafeQueue::signal() { queue_empty_cv.notify_one(); }
