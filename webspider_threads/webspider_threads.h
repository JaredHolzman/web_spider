#ifndef _WEBSPIDER_THREADS_H
#define _WEBSPIDER_THREADS_H

#include "../threadsafe_queue/threadsafe_queue.h"
#include "../webpage_scraper/webpage_scraper.h"
#include <condition_variable>
#include <err.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <string.h>
#include <string>
#include <system_error>
#include <thread>


class WebspiderThreads {
public:
  WebspiderThreads(std::string root_webpage_address, size_t max_threads,
                   int max_depth,
                   std::unique_ptr<ThreadsafeQueue<Page>> tsqueue,
                   std::unique_ptr<WebPageScraper> scraper);

  ~WebspiderThreads();

  void crawl_web();

private:
  void crawl_page();

  std::string root_webpage_address;
  size_t max_threads;
  int max_depth;
  std::unique_ptr<ThreadsafeQueue<Page>> tsqueue;
  std::unique_ptr<WebPageScraper> scraper;
  std::mutex finished_mutex;
  bool is_finished;
  std::condition_variable avail_threads_cv;
  size_t avail_threads;
};

#endif
