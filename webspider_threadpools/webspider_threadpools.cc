#include "webspider_threadpools.h"

ThreadsafeQueue *tsqueue_curr; // Links at current depth to explore
ThreadsafeQueue *tsqueue_next; // Links at next depth
WebPageScraper *scraper;
int *max_depth;
int *items_to_remove;
pthread_mutex_t lock;
pthread_cond_t items;

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
  pthread_cond_init(&items, NULL);

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
    printf("WebCrawler: completed join with thread %d having a status of %ld\n",
           i, (long)status);
  }
}

void *WebspiderThreadpools::crawl_page(void *threadID) {
  // std::cout << *max_depth << std::endl;
  while (*max_depth > 0) {

    // Reserve item to remove
    pthread_mutex_lock(&lock);
    while (items_to_remove == 0) {
      if (max_depth == 0) {
        pthread_mutex_unlock(&lock);
        pthread_exit(threadID);
      }
      pthread_cond_wait(&items, &lock);
    }
    *items_to_remove -= 1;
    pthread_mutex_unlock(&lock);

    // Process first page in queue
    process_page();

    // If the current depth has been explored entirely and we have not reached
    // the max depth, move to the next layer and decrement max_depth.
    pthread_mutex_lock(&lock);
    *max_depth -= 1;
    std::cout << *max_depth << std::endl;
    if (tsqueue_next->isEmpty() && *max_depth > 0) {
      tsqueue_curr = tsqueue_next;
      tsqueue_next->clear();
      *items_to_remove = tsqueue_curr->size();
    } else if (*max_depth <= 0) {
      pthread_cond_broadcast(&items);
    }
    pthread_mutex_unlock(&lock);
  }
  pthread_exit(threadID);
}

void WebspiderThreadpools::process_page() {
  std::string *webpage_address = tsqueue_curr->remove();
  std::vector<std::string *> linked_pages =
      scraper->get_page_hrefs(*webpage_address);

  for (size_t i = 0; i < linked_pages.size(); i++) {
    // std::cout << *linked_pages[i] << std::endl;
    // Append linked pages to queue representing next the next layer. This
    // allows for depth to be tracked.
    tsqueue_next->append(linked_pages[i]);
  }
}
