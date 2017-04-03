#include "webspider_threadpools.h"

ThreadsafeQueue *tp_tsqueue;
WebPageScraper *tp_scraper;
int *tp_max_depth;
std::mutex tp_mutex;
bool tp_finished;

void WebspiderThreadpools::CrawlWeb(std::string *root_webpage_address,
                                    int max_threads, int _max_depth,
                                    ThreadsafeQueue *_tsqueue,
                                    WebPageScraper *_scraper) {

  // Initialize global variables and lock
  tp_tsqueue = _tsqueue;
  tp_scraper = _scraper;
  tp_max_depth = new int(_max_depth);
  tp_finished = false;

  std::thread workers[max_threads];

  tp_tsqueue->append(
      new Page(root_webpage_address, new std::string("Root"), 0));

  // Create threads to crawl webpages
  for (size_t i = 0; i < max_threads; i++) {
    workers[i] = std::thread(crawl_page, i);
  }

  // Wait for threads to complete
  for (size_t i = 0; i < max_threads; i++) {
    WebspiderThreadpools::join_workers(workers[i], true);
  }
}

void *WebspiderThreadpools::crawl_page(size_t threadID) {
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

  std::unique_lock<std::mutex> lock(tp_mutex);
  if (!tp_finished) {
    tp_finished = true;
    tp_tsqueue->setFinished();
  }
  lock.unlock();
  tp_tsqueue->signal();
}

void WebspiderThreadpools::join_workers(std::thread &thread, bool verbose) {
  try {
    thread.join();
    if (verbose) {
      std::cout << "Spider: completed join with thread " << thread.get_id()
                << std::endl;
    }
  } catch (const std::system_error &e) {
    std::cout << "ERROR: Caught system_error with code " << e.code()
              << " meaning " << e.what() << '\n';
  }
}
