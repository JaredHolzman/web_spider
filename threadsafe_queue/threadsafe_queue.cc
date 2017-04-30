#include "threadsafe_queue.h"

template <class T>
ThreadsafeQueue<T>::ThreadsafeQueue()
    : queue(), queue_mutex(), queue_empty_cv(), finished(false) {}

template <class T>
ThreadsafeQueue<T>::~ThreadsafeQueue() {}

template <class T>
void ThreadsafeQueue<T>::append(std::unique_ptr<T> elem) {
  std::unique_lock<std::mutex> lock(queue_mutex);
  queue.push_back(std::move(elem));
  lock.unlock();
  queue_empty_cv.notify_one();
}

template <class T>
std::unique_ptr<T> ThreadsafeQueue<T>::remove() {
  std::unique_lock<std::mutex> lock(queue_mutex);
  while (queue.empty()) {
    if (finished) {
      lock.unlock();
      return NULL;
    }
    queue_empty_cv.wait(lock);
  }

  std::unique_ptr<T> head = std::move(queue.front());
  queue.erase(queue.begin());
  lock.unlock();

  return head;
}

template <class T>
bool ThreadsafeQueue<T>::isEmpty() {
  return queue.empty();
}

template <class T>
void ThreadsafeQueue<T>::setFinished() {
  finished = true;
};

template <class T>
void ThreadsafeQueue<T>::signal() {
  queue_empty_cv.notify_one();
}

template class ThreadsafeQueue<Page>;
