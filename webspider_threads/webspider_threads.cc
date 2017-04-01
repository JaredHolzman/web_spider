#include "webspider_threads.h"

ThreadsafeQueue *t_tsqueue;
WebPageScraper *t_scraper;
int t_max_depth;
size_t t_num_threads;
bool t_finished;
pthread_mutex_t t_lock;
pthread_cond_t t_max_threads_cond;

void WebspiderThreads::CrawlWeb(std::string *root_webpage_address,
                                int max_threads, int _max_depth,
                                ThreadsafeQueue *_tsqueue,
                                WebPageScraper *_scraper) {

  // Initialize global variables and lock
  t_tsqueue = _tsqueue;
  t_scraper = _scraper;
  t_max_depth = _max_depth;
  t_finished = false;
  t_num_threads = max_threads;

  pthread_mutex_init(&t_lock, NULL);
  pthread_cond_init(&t_max_threads_cond, NULL);

  pthread_t new_thread;
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  t_tsqueue->append(new Page(root_webpage_address, new std::string("ROOT"), 0));

  while (!t_tsqueue->isEmpty() || !t_finished) {
    pthread_mutex_lock(&t_lock);
    // Max number of threads are currently running
    while (t_num_threads == 0) {
      pthread_cond_wait(&t_max_threads_cond, &t_lock);
    }
    pthread_create(&new_thread, &attr, crawl_page, (void *)(t_num_threads));
    t_num_threads--;
    pthread_mutex_unlock(&t_lock);
  }

  pthread_mutex_lock(&t_lock);
  // While there are still threads running, wait
  while (t_num_threads < max_threads) {
    pthread_cond_wait(&t_max_threads_cond, &t_lock);
  }
  pthread_mutex_unlock(&t_lock);
}

void *WebspiderThreads::crawl_page(void *threadID) {
  Page *page = t_tsqueue->remove();

  if (page == NULL) {
    t_tsqueue->signal();
    pthread_mutex_lock(&t_lock);
    t_num_threads++;
    pthread_cond_signal(&t_max_threads_cond);
    pthread_mutex_unlock(&t_lock);
    pthread_exit(threadID);
  }

  std::vector<std::string *> linked_pages =
      t_scraper->get_page_hrefs(*page->get_href());

  int depth_next;
  for (size_t i = 0; i < linked_pages.size(); i++) {
    std::cout << *linked_pages[i] << std::endl;
    if ((depth_next = page->get_depth() + 1) < t_max_depth) {
      t_tsqueue->append(
          new Page(linked_pages[i], page->get_href(), depth_next));
    }
  }

  pthread_mutex_lock(&t_lock);
  if (depth_next >= t_max_depth) {
    if (!t_finished) {
      t_finished = true;
      t_tsqueue->setFinished();
    }
    t_tsqueue->signal();
  }

  t_num_threads++;
  pthread_cond_signal(&t_max_threads_cond);
  pthread_mutex_unlock(&t_lock);

  pthread_exit(threadID);
}

void WebspiderThreads::join_workers(pthread_t thread, bool verbose) {
  void *status;
  int rc = pthread_join(thread, &status);

  if (rc) {
    printf("ERROR; return code from pthread_join() is %s\n", strerror(rc));
    exit(-1);
  }
  if (verbose) {
    printf("Spider: completed join with thread %ld\n", (long)status);
  }
}
