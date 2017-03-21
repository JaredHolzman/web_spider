#include "threadsafe_queue/threadsafe_queue.h"
#include "webpage_scraper/webpage_scraper.h"
#include "webspider_threadpools/webspider_threadpools.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  // WebPageScraper scraper = WebPageScraper();
  // Not actually the hrefs for now, just HTML blob
  // std::vector<std::string *> hrefs = scraper.get_page_hrefs(argv[1]);

  // for(std::string *s: hrefs){
  // std::cout << *s << std::endl;
  // }

  ThreadsafeQueue *tsqueue1 = new ThreadsafeQueue();
  ThreadsafeQueue *tsqueue2 = new ThreadsafeQueue();
  WebPageScraper *scraper = new WebPageScraper();

  WebspiderThreadpools::CrawlWeb(new std::string(argv[1]), atoi(argv[2]),
                                      atoi(argv[3]), tsqueue1, tsqueue2,
                                      scraper);
}
