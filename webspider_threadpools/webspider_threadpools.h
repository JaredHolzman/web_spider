#ifndef _WEBSPIDER_THREADPOOLS_H
#define _WEBSPIDER_THREADPOOLS_H

#include "../threadsafe_queue/threadsafe_queue.h"
#include "../webpage_scraper/webpage_scraper.h"
#include <iostream>
#include <string>

namespace WebspiderThreadpools {

void CrawlWeb(std::string *root_webpage_address, int _max_threads,
                   int max_depth, ThreadsafeQueue *_tsqueue1,
                   ThreadsafeQueue *_tsqueue2, WebPageScraper *_scraper);
void *crawl_page(void *threadID);
}

#endif
