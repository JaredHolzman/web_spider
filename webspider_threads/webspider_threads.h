#ifndef _WEBSPIDER_THREADS_H
#define _WEBSPIDER_THREADS_H

#include "../threadsafe_queue/threadsafe_queue.h"
#include "../webpage_scraper/webpage_scraper.h"
#include <iostream>
#include <pthread.h>
#include <string>

namespace WebspiderThreads {
void CrawlWeb(std::string *root_webpage_address, int _max_threads,
              int max_depth, ThreadsafeQueue *_tsqueue,
              WebPageScraper *_scraper);
void *crawl_page(void *threadID);
  void join_workers(pthread_t thread, bool verbose);
}

#endif
