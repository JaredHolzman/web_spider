#ifndef _WEBSPIDER_THREADPOOLS_H
#define _WEBSPIDER_THREADPOOLS_H

#include "../threadsafe_queue/threadsafe_queue.h"
#include "../webpage_scraper/webpage_scraper.h"
#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <system_error>
#include <thread>

class WebspiderThreadpools {
public:
  WebspiderThreadpools(std::string root_webpage_address, int max_threads,
                       int max_depth, std::unique_ptr<ThreadsafeQueue> tsqueue,
                       std::unique_ptr<WebPageScraper> scraper);

  ~WebspiderThreadpools();
  void CrawlWeb();

private:
  void crawl_page();
  void join_workers(std::thread &thread, bool verbose);

  std::string root_webpage_address;
  int max_threads;
  int max_depth;
  std::unique_ptr<ThreadsafeQueue> tsqueue;
  std::unique_ptr<WebPageScraper> scraper;
  std::mutex finished_mutex;
  bool is_finished;
};

#endif
