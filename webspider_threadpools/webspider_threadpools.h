#ifndef _WEBSPIDER_THREADPOOLS_H
#define _WEBSPIDER_THREADPOOLS_H

#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <thread>
#include "../threadsafe_queue/threadsafe_queue.h"
#include "../webpage_scraper/webpage_scraper.h"

class WebspiderThreadpools {
 public:
  WebspiderThreadpools(const std::string &root_webpage_address,
                       const size_t max_threads, const int max_depth,
                       ThreadsafeQueue<Page> &tsqueue, HTMLScraper &scraper,
                       const bool verbose);

  ~WebspiderThreadpools();

  void crawl_web();

 private:
  void crawl_page();
  void join_workers(std::thread &thread, bool verbose);

  const std::string &root_webpage_address;
  const size_t max_threads;
  const int max_depth;
  const bool verbose;
  ThreadsafeQueue<Page> &tsqueue;
  HTMLScraper &scraper;
  std::mutex finished_mutex;
  bool is_finished;
};

#endif
