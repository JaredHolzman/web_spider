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

  std::vector<pthread_t> workers;

  tsqueue->append(new Page(root_webpage_address, 0));

  while (!tsqueue->isEmpty()) {
    // If the max number of threads has been reached, join the first thread in
    // the list
    if (workers.size() == (size_t)max_threads) {
      void *status;
      int rc = pthread_join(workers.front(), &status);

      if (rc) {
        printf("ERROR; return code from pthread_join() is %d\n", rc);
        exit(-1);
      }
      printf("Spider: completed join with thread %ld\n", (long)status);
    }

    pthread_create(&workers.back(), NULL, crawl_page, (void *)(workers.size()));
  }
}


void *WebspiderThreads::crawl_page(void *threadID) {
    Page *page = tsqueue->remove();

    std::vector<std::string *> linked_pages =
      scraper->get_page_hrefs(*page->get_href());
    for (size_t i = 0; i < linked_pages.size(); i++) {
      std::cout << *linked_pages[i] << std::endl;
      int depth_next;
      if ((depth_next = page->get_depth() + 1) < *max_depth) {
        tsqueue->append(new Page(linked_pages[i], depth_next));
      }
    }
  pthread_exit(threadID);
}
