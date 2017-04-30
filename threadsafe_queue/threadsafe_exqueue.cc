#include "threadsafe_exqueue.h"

template <class T>
ThreadsafeExQueue<T>::ThreadsafeExQueue() : exclude_set(), set_mutex() {}

template <class T>
ThreadsafeExQueue<T>::~ThreadsafeExQueue() {}

template <class T>
void ThreadsafeExQueue<T>::append(std::unique_ptr<T> elem) {
  std::unique_lock<std::mutex> set_lock(set_mutex);
  if (exclude_set.count(*elem)) {
    set_lock.unlock();
    return;
  }

  exclude_set.insert(T(*elem));
  set_lock.unlock();

  std::unique_lock<std::mutex> q_lock(ThreadsafeQueue<T>::queue_mutex);
  ThreadsafeQueue<T>::queue.push_back(std::move(elem));
  q_lock.unlock();
  ThreadsafeQueue<T>::queue_empty_cv.notify_one();
}

template <class T>
std::unique_ptr<T> ThreadsafeExQueue<T>::remove() {
  std::unique_lock<std::mutex> lock(ThreadsafeQueue<T>::queue_mutex);
  while (ThreadsafeQueue<T>::queue.empty()) {
    if (ThreadsafeQueue<T>::finished) {
      lock.unlock();
      return NULL;
    }
    ThreadsafeQueue<T>::queue_empty_cv.wait(lock);
  }

  std::unique_ptr<T> head = std::move(ThreadsafeQueue<T>::queue.front());
  ThreadsafeQueue<T>::queue.erase(ThreadsafeQueue<T>::queue.begin());
  lock.unlock();

  return head;
}

template class ThreadsafeExQueue<Page>;
