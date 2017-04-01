#include "webspider_threads.h"

ThreadsafeQueue *t_tsqueue;
WebPageScraper *t_scraper;
int *t_max_depth;
pthread_mutex_t t_lock;
pthread_cond_t t_thread_finished;
int t_thread_number;
bool t_finished;
size_t t_finished_thread_id;

void WebspiderThreads::CrawlWeb(std::string *root_webpage_address,
                                int max_threads, int _max_depth,
                                ThreadsafeQueue *_tsqueue,
                                WebPageScraper *_scraper) {

  // Initialize global variables and lock
  t_tsqueue = _tsqueue;
  t_scraper = _scraper;
  t_max_depth = new int(_max_depth);
  t_finished = false;
  t_thread_number = 0;
  t_finished_thread_id = -1;

  pthread_mutex_init(&t_lock, NULL);
  pthread_cond_init(&t_thread_finished, NULL);

  std::vector<pthread_t> workers;
  pthread_t new_thread;

  t_tsqueue->append(new Page(root_webpage_address, new std::string("ROOT"), 0));

  while (!t_finished) {
    // If the max number of threads has been reached, join a finished thread and
    // create a new thread in its place
    // if (workers.size() == (size_t)max_threads) {
    //   size_t id;
    //   pthread_mutex_lock(&t_lock);
    //   while (t_finished_thread_id == -1) {
    //     pthread_cond_wait(&t_thread_finished, &t_lock);
    //   }
    //   id = t_finished_thread_id;
    //   t_finished_thread_id = -1;
    //   pthread_cond_signal(&t_thread_finished);
    //   pthread_mutex_unlock(&t_lock);

    //   WebspiderThreads::join_workers(workers[id], false);
    //   pthread_create(&new_thread, NULL, crawl_page, (void *)(id));
    //   workers[id] = new_thread;
    // } else {
    // }

    pthread_create(&new_thread, NULL, crawl_page, (void *)(workers.size()));
    workers.push_back(new_thread);
  }

  for (size_t i = 0; i < workers.size(); i++) {
    WebspiderThreads::join_workers(workers[i], true);
  }
}

void *WebspiderThreads::crawl_page(void *threadID) {
  // pthread_mutex_lock(&t_lock);
  // std::cout << threadID << std::endl;
  // pthread_mutex_unlock(&t_lock);

  Page *page = t_tsqueue->remove();

  if (page == NULL) {
    t_tsqueue->signal();
    pthread_exit(threadID);
  }

  std::vector<std::string *> linked_pages =
      t_scraper->get_page_hrefs(*page->get_href());

  int depth_next;
  for (size_t i = 0; i < linked_pages.size(); i++) {
    std::cout << *linked_pages[i] << std::endl;
    if ((depth_next = page->get_depth() + 1) < *t_max_depth) {
      t_tsqueue->append(
          new Page(linked_pages[i], page->get_href(), depth_next));
    }
  }

  pthread_mutex_lock(&t_lock);
  if (depth_next >= *t_max_depth) {
    if (!t_finished) {
      t_finished = true;
      t_tsqueue->setFinished();
    }
    t_tsqueue->signal();
  }

  // while (t_finished_thread_id != -1) {
  //   pthread_cond_wait(&t_thread_finished, &t_lock);
  // }
  // t_finished_thread_id = (size_t)threadID;
  // pthread_cond_signal(&t_thread_finished);
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
