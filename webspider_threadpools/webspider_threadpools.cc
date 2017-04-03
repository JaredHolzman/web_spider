#include "webspider_threadpools.h"

ThreadsafeQueue *tp_tsqueue;
WebPageScraper *tp_scraper;
int *tp_max_depth;
pthread_mutex_t tp_lock;
bool tp_finished;

void WebspiderThreadpools::CrawlWeb(std::string *root_webpage_address,
                                    int max_threads, int _max_depth,
                                    ThreadsafeQueue *_tsqueue,
                                    WebPageScraper *_scraper) {

  // Initialize global variables and lock
  tp_tsqueue = _tsqueue;
  tp_scraper = _scraper;
  tp_max_depth = new int(_max_depth);
  pthread_mutex_init(&tp_lock, NULL);
  tp_finished = false;

  pthread_t workers[max_threads];

  tp_tsqueue->append(
      new Page(root_webpage_address, new std::string("Root"), 0));

  // Create threads to crawl webpages
  for (long i = 0; i < max_threads; i++) {
    pthread_create(&workers[i], NULL, crawl_page, (void *)i);
  }

  // Wait for threads to complete
  for (int i = 0; i < max_threads; i++) {
    WebspiderThreadpools::join_workers(workers[i], true);
  }
}

void *WebspiderThreadpools::crawl_page(void *threadID) {
  while (!tp_finished) {
    Page *page = tp_tsqueue->remove();

    if (page == NULL) {
      break;
    }

    std::vector<std::string *> linked_pages =
        tp_scraper->get_page_hrefs(*page->get_href());

    int depth_next = depth_next = page->get_depth() + 1;
    for (size_t i = 0; i < linked_pages.size(); i++) {
      std::cout << *linked_pages[i] << " " << depth_next << std::endl;
      if (depth_next < *tp_max_depth) {
        tp_tsqueue->append(
            new Page(linked_pages[i], page->get_href(), depth_next));
      }
    }

    if (depth_next >= *tp_max_depth) {
      break;
    }
  }

  pthread_mutex_lock(&tp_lock);
  if (!tp_finished) {
    tp_finished = true;
    tp_tsqueue->setFinished();
  }
  tp_tsqueue->signal();
  pthread_mutex_unlock(&tp_lock);

  pthread_exit(threadID);
}

void WebspiderThreadpools::join_workers(pthread_t thread, bool verbose) {
  void *status;
  int rc = pthread_join(thread, &status);

  if (rc) {
    printf("ERROR; return code from pthread_join() is %d\n", rc);
    exit(-1);
  }
  if (verbose) {
    printf("Spider: completed join with thread %ld\n", (long)status);
  }
}
