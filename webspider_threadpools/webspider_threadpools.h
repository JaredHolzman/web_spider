#ifndef _WEBSPIDER_THREADPOOLS_H
#define _WEBSPIDER_THREADPOOLS_H

#include "../threadsafe_queue/threadsafe_queue.h"
#include "../webpage_scraper/webpage_scraper.h"
#include <condition_variable>
#include <system_error>
#include <iostream>
#include <mutex>
#include <string>
#include <thread>

namespace WebspiderThreadpools {
void CrawlWeb(std::string *root_webpage_address, int _max_threads,
              int max_depth, ThreadsafeQueue *_tsqueue,
              WebPageScraper *_scraper);
void *crawl_page(size_t threadID);
void join_workers(std::thread &thread, bool verbose);
}

#endif
