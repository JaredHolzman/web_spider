#ifndef _WEBSPIDER_THREADS_H
#define _WEBSPIDER_THREADS_H

#include "../threadsafe_queue/threadsafe_queue.h"
#include "../webpage_scraper/webpage_scraper.h"
#include <iostream>
#include <string>

class WebSpiderThreads {
public:
  WebSpiderThreads(int max_threads);
  void crawl_web(std::string root_webpage_address);

private:
  int max_threads; // Number of threads running at once
  static ThreadsafeQueue *tsqueue;
  static WebPageScraper scraper;

  static void *crawl_page(void *threadID);
};

#endif
