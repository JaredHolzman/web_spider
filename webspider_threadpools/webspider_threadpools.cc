#include "webspider_threadpools.h"

ThreadsafeQueue *tsqueue_curr; // Links at current depth to explore
ThreadsafeQueue *tsqueue_next; // Links at next depth
WebPageScraper *scraper;
int *max_depth;
int *items_to_remove;
pthread_mutex_t lock;

void WebspiderThreadpools::CrawlWeb(std::string *root_webpage_address,
                                    int max_threads, int _max_depth,
                                    ThreadsafeQueue *_tsqueue1,
                                    ThreadsafeQueue *_tsqueue2,
                                    WebPageScraper *_scraper) {

  // Initialize global variables and lock
  tsqueue_curr = _tsqueue1;
  tsqueue_next = _tsqueue2;
  scraper = _scraper;
  max_depth = new int(_max_depth);
  pthread_mutex_init(&lock, NULL);

  tsqueue_curr->append(root_webpage_address);
  items_to_remove = new int(tsqueue_curr->size());

  // Generate max_threads number of threads to crawl webpages
  pthread_t workers[max_threads];
  for (long i = 0; i < max_threads; i++) {
    pthread_create(&workers[i], NULL, crawl_page, (void *)i);
  }

  void *status;
  // Wait for threads to complete
  for (int i = 0; i < max_threads; i++) {
    int rc = pthread_join(workers[i], &status);
    if (rc) {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
    printf("Producer: completed join with thread %d having a status of %ld\n",
           i, (long)status);
  }
}

void *WebspiderThreadpools::crawl_page(void *threadID) {
  while (!tsqueue_curr->isEmpty() && *max_depth > 0) {

    // Reserve item to remove
    pthread_mutex_lock(&lock);
    if (*items_to_remove == 0) {
      pthread_mutex_unlock(&lock);
      break;
    }
    *items_to_remove -= 1;
    pthread_mutex_unlock(&lock);

    std::string *webpage_address = tsqueue_curr->remove();
    std::vector<std::string *> linked_pages =
        scraper->get_page_hrefs(*webpage_address);

    for (size_t i = 0; i < linked_pages.size(); i++) {
      std::cout << *linked_pages[i] << std::endl;
      // Append linked pages to queue representing next the next layer. This
      // allows for depth to be tracked.
      tsqueue_next->append(linked_pages[i]);
    }

    // If the current depth has been explored entirely, move to the next layer
    // and decrement max_depth.
    pthread_mutex_unlock(&lock);
    if (tsqueue_next->isEmpty()) {
      tsqueue_curr = tsqueue_next;
      tsqueue_next->clear();
      *max_depth -= 1;
    }
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(threadID);
}
