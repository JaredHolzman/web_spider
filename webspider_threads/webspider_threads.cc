#include "webspider_threads.h"

ThreadsafeQueue *tsqueue;
WebPageScraper *scraper;
int *max_depth;
pthread_mutex_t lock;

void WebspiderThreads::CrawlWeb(std::string *root_webpage_address,
                                int max_threads, int _max_depth,
                                ThreadsafeQueue *_tsqueue,
                                WebPageScraper *_scraper) {

  // Initialize global variables and lock
  tsqueue = _tsqueue;
  scraper = _scraper;
  max_depth = new int(_max_depth);
  pthread_mutex_init(&lock, NULL);
}
