#include "threadsafe_queue/threadsafe_exqueue.h"
#include "threadsafe_queue/threadsafe_queue.h"
#include "webpage_scraper/webpage_scraper.h"
#include "webspider_threadpools/webspider_threadpools.h"
#include "webspider_threads/webspider_threads.h"

#include <string.h>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char *argv[]) {
  if (argc != 5 && argc != 6) {
    std::cout << "Usage: ./spider [http://www.example.com webpage_address] [1 "
                 "max_threads] [1 max_depth] [t|p] [v]"
              << std::endl;

    return 0;
  }

  std::string root_webpage_address = std::string(argv[1]);
  ThreadsafeExQueue<Page> ts_queue;
  HTMLScraper scraper(root_webpage_address);
  bool verbose = argc == 6 && *argv[5] == 'v';

  if (strcmp(argv[4], "t") == 0) {
    WebspiderThreads spider(root_webpage_address, atoi(argv[2]), atoi(argv[3]),
                            ts_queue, scraper, verbose);
    spider.crawl_web();
  } else if (strcmp(argv[4], "p") == 0) {
    WebspiderThreadpools spider(root_webpage_address, atoi(argv[2]),
                                atoi(argv[3]), ts_queue, scraper, verbose);
    spider.crawl_web();
  }
}
