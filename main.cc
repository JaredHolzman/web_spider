#include "threadsafe_queue/threadsafe_queue.h"
#include "webpage_scraper/webpage_scraper.h"
#include "webspider_threadpools/webspider_threadpools.h"
#include "webspider_threads/webspider_threads.h"

#include <iostream>
#include <memory>
#include <string.h>
#include <string>

int main(int argc, char *argv[]) {
  if (strcmp(argv[4], "t") == 0) {
    WebspiderThreads t(std::string(argv[1]), atoi(argv[2]), atoi(argv[3]),
                       std::unique_ptr<ThreadsafeQueue>(new ThreadsafeQueue()),
                       std::unique_ptr<WebPageScraper>(new WebPageScraper()));
    t.crawl_web();
  } else if (strcmp(argv[4], "p") == 0) {
    WebspiderThreadpools t(
        std::string(argv[1]), atoi(argv[2]), atoi(argv[3]),
        std::unique_ptr<ThreadsafeQueue>(new ThreadsafeQueue()),
        std::unique_ptr<WebPageScraper>(new WebPageScraper()));
    t.crawl_web();
  }
}
