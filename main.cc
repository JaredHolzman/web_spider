#include "threadsafe_queue/threadsafe_queue.h"
#include "webpage_scraper/webpage_scraper.h"
#include "webspider_threadpools/webspider_threadpools.h"
#include "webspider_threads/webspider_threads.h"

#include <iostream>
#include <string.h>
#include <string>

int main(int argc, char *argv[]) {
  ThreadsafeQueue *tsqueue = new ThreadsafeQueue();
  WebPageScraper *scraper = new WebPageScraper();

  // if (strcmp(argv[4], "t") == 0) {
  //   WebspiderThreads::CrawlWeb(new std::string(argv[1]), atoi(argv[2]),
  //                              atoi(argv[3]), tsqueue, scraper);
  // } else if (strcmp(argv[4], "p") == 0) {
  //   WebspiderThreadpools::CrawlWeb(new std::string(argv[1]), atoi(argv[2]),
  //                                  atoi(argv[3]), tsqueue, scraper);
  // }
}
