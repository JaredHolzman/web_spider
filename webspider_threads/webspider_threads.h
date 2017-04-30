#ifndef _WEBSPIDER_THREADS_H
#define _WEBSPIDER_THREADS_H

#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <thread>
#include "../threadsafe_queue/threadsafe_queue.h"
#include "../webpage_scraper/webpage_scraper.h"

class WebspiderThreads {
 public:
  WebspiderThreads(const std::string &root_webpage_address,
                   const size_t max_threads, const int max_depth,
                   ThreadsafeQueue<Page> &tsqueue, HTMLScraper &scraper,
                   const bool verbose);

  ~WebspiderThreads();

  void crawl_web();

 private:
  void crawl_page();

  const std::string &root_webpage_address;
  const size_t max_threads;
  const int max_depth;
  const bool verbose;
  ThreadsafeQueue<Page> &tsqueue;
  HTMLScraper &scraper;
  std::mutex finished_mutex;
  bool is_finished;
  std::condition_variable avail_threads_cv;
  size_t avail_threads;
};

#endif
