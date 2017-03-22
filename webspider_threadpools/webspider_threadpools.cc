#include "webspider_threadpools.h"

ThreadsafeQueue *tp_tsqueue;
WebPageScraper *tp_scraper;
int *tp_max_depth;
pthread_mutex_t tp_lock;

void WebspiderThreadpools::CrawlWeb(std::string *root_webpage_address, int max_threads,
                   int _max_depth, ThreadsafeQueue *_tsqueue,
                   WebPageScraper *_scraper) {

  // Initialize global variables and lock
  tp_tsqueue = _tsqueue;
  tp_scraper = _scraper;
  tp_max_depth = new int(_max_depth);
  pthread_mutex_init(&tp_lock, NULL);

  pthread_t workers[max_threads];
  void *status;

  tp_tsqueue->append(new Page(root_webpage_address, 0));

  // Create threads to crawl webpages
  for (long i = 0; i < max_threads; i++) {
    pthread_create(&workers[i], NULL, crawl_page, (void *)i);
  }

  // Wait for threads to complete
  for (int i = 0; i < max_threads; i++) {
    int rc = pthread_join(workers[i], &status);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Spider: completed join with thread %d having a status of %ld\n", i,
           (long)status);
  }
}

void *WebspiderThreadpools::crawl_page(void *threadID) {
  while (!tp_tsqueue->isEmpty()) {
    Page *page = tp_tsqueue->remove();

    std::vector<std::string *> linked_pages =
      tp_scraper->get_page_hrefs(*page->get_href());
    for (size_t i = 0; i < linked_pages.size(); i++) {
      std::cout << *linked_pages[i] << std::endl;
      int depth_next;
      if ((depth_next = page->get_depth() + 1) < *tp_max_depth) {
        tp_tsqueue->append(new Page(linked_pages[i], depth_next));
      }
    }
  }
  pthread_exit(threadID);
}
