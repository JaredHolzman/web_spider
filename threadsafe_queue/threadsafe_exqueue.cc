#include "threadsafe_exqueue.h"

template <class T>
ThreadsafeExQueue<T>::ThreadsafeExQueue() : exclude_set(), set_mutex() {}

template <class T> ThreadsafeExQueue<T>::~ThreadsafeExQueue() {}

template <class T> void ThreadsafeExQueue<T>::append(T *page_href) {
  std::unique_lock<std::mutex> set_lock(ThreadsafeQueue<T>::queue_mutex);
  if (exclude_set.count(*page_href)) {
    set_lock.unlock();
    delete page_href;
    return;
  }
  exclude_set.insert(*page_href);
  set_lock.unlock();

  std::unique_lock<std::mutex> q_lock(ThreadsafeQueue<T>::queue_mutex);
  ThreadsafeQueue<T>::queue.push_back(page_href);
  q_lock.unlock();
  ThreadsafeQueue<T>::queue_empty_cv.notify_one();
}

template <class T> T *ThreadsafeExQueue<T>::remove() {
  std::unique_lock<std::mutex> lock(ThreadsafeQueue<T>::queue_mutex);
  while (ThreadsafeQueue<T>::queue.empty()) {
    if (ThreadsafeQueue<T>::finished) {
      lock.unlock();
      return NULL;
    }
    ThreadsafeQueue<T>::queue_empty_cv.wait(lock);
  }

  T *head = ThreadsafeQueue<T>::queue.front();
  ThreadsafeQueue<T>::queue.erase(ThreadsafeQueue<T>::queue.begin());
  lock.unlock();

  return head;
}

template class ThreadsafeExQueue<Page>;
