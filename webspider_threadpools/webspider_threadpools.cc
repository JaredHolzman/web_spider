#include "webspider_threadpools.h"

ThreadsafeQueue *tsqueue;
WebPageScraper *scraper;
int *max_depth;
pthread_mutex_t lock;

void CrawlWebPools(std::string *root_webpage_address, int max_threads,
                   int _max_depth, ThreadsafeQueue *_tsqueue,
                   WebPageScraper *_scraper) {

  // Initialize global variables and lock
  tsqueue = _tsqueue;
  scraper = _scraper;
  max_depth = new int(_max_depth);
  pthread_mutex_init(&lock, NULL);

  pthread_t workers[max_threads];
  void *status;

  tsqueue->append(root_webpage_address);
  tsqueue->append(NULL);

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
    printf("Spider: completed join with thread %d having a status of %ld\n",
           i, (long)status);
  }
}

void *crawl_page(void *threadID) {
  while (max_depth > 0) {
    // Exit if max depth has been reached
    pthread_mutex_lock(&lock);
    if (*max_depth <= 0) {
      pthread_mutex_unlock(&lock);
      break;
    }
    // *max_depth -= 1;
    pthread_mutex_unlock(&lock);

    std::string *webpage_address = tsqueue->remove();

    // Decremenent depth every time a depth marker is hit
    while (webpage_address == NULL) {
      pthread_mutex_lock(&lock);
      // if (*max_depth == 0) {
      //   pthread_mutex_unlock(&lock);
      //   pthread_exit(threadID);
      // }
      *max_depth -= 1;
      // std::cout << *max_depth << std::endl;
      pthread_mutex_unlock(&lock);
      webpage_address = tsqueue->remove();
    }

    std::vector<std::string *> linked_pages =
        scraper->get_page_hrefs(*webpage_address);
    for (size_t i = 0; i < linked_pages.size(); i++) {
      std::cout << *linked_pages[i] << std::endl;
      tsqueue->append(linked_pages[i]);
    }
    tsqueue->append(NULL);
  }
  pthread_exit(threadID);
}
