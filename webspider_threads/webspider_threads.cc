#include "webspider_threads.h"

ThreadsafeQueue *t_tsqueue;
WebPageScraper *t_scraper;
int *t_max_depth;
pthread_mutex_t t_lock;

void WebspiderThreads::CrawlWeb(std::string *root_webpage_address,
                                int max_threads, int _max_depth,
                                ThreadsafeQueue *_tsqueue,
                                WebPageScraper *_scraper) {

  // Initialize global variables and lock
  t_tsqueue = _tsqueue;
  t_scraper = _scraper;
  t_max_depth = new int(_max_depth);
  pthread_mutex_init(&t_lock, NULL);

  std::vector<pthread_t> workers;

  t_tsqueue->append(new Page(root_webpage_address, 0));

  while (!t_tsqueue->isEmpty()) {
    // If the max number of threads has been reached, join the first thread in
    // the list
    if (workers.size() == (size_t)max_threads) {
      WebspiderThreads::join_workers(workers.front());
    }
    pthread_create(&workers.back(), NULL, crawl_page, (void *)(workers.size()));
  }

  for(pthread_t tr: workers){
    WebspiderThreads::join_workers(tr);
  }
}

void *WebspiderThreads::crawl_page(void *threadID) {
  Page *page = t_tsqueue->remove();

  std::vector<std::string *> linked_pages =
      t_scraper->get_page_hrefs(*page->get_href());
  for (size_t i = 0; i < linked_pages.size(); i++) {
    std::cout << *linked_pages[i] << std::endl;
    int depth_next;
    if ((depth_next = page->get_depth() + 1) < *t_max_depth) {
      t_tsqueue->append(new Page(linked_pages[i], depth_next));
    }
  }
  pthread_exit(threadID);
}

void WebspiderThreads::join_workers(pthread_t thread) {
  void *status;
  int rc = pthread_join(thread, &status);

  if (rc) {
    printf("ERROR; return code from pthread_join() is %d\n", rc);
    exit(-1);
  }
  printf("Spider: completed join with thread %ld\n", (long)status);
}
