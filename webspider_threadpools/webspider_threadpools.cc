#include "webspider_threadpools.h"

WebspiderThreadpools::WebspiderThreadpools(
    std::string root_webpage_address, int max_threads, int max_depth,
    std::unique_ptr<ThreadsafeQueue<Page>> tsqueue,
    std::unique_ptr<WebPageScraper> scraper)
    : root_webpage_address(root_webpage_address), max_threads(max_threads),
      max_depth(max_depth), tsqueue(std::move(tsqueue)),
      scraper(std::move(scraper)), finished_mutex(), is_finished(false) {}

WebspiderThreadpools::~WebspiderThreadpools() {}

void WebspiderThreadpools::crawl_web() {
  std::thread workers[max_threads];

  tsqueue->append(new Page(&root_webpage_address, new std::string("Root"), 0));

  // Create threads to crawl webpages
  for (size_t i = 0; i < max_threads; i++) {
    workers[i] = std::thread(&WebspiderThreadpools::crawl_page, this);
  }

  // Wait for threads to complete
  for (size_t i = 0; i < max_threads; i++) {
    WebspiderThreadpools::join_workers(workers[i], false);
  }
}

void WebspiderThreadpools::crawl_page() {
  while (!is_finished) {
    Page *page = tsqueue->remove();

    if (page == NULL) {
      break;
    }

    std::vector<std::string *> linked_pages =
        scraper->get_page_hrefs(*page->page_href);

    int depth_next = page->depth + 1;
    for (size_t i = 0; i < linked_pages.size(); i++) {
      std::cout << *linked_pages[i] << " " << depth_next << std::endl;
      if (depth_next < max_depth) {
        tsqueue->append(new Page(linked_pages[i], page->page_href, depth_next));
      }
    }

    if (depth_next >= max_depth) {
      break;
    }
  }

  std::unique_lock<std::mutex> lock(finished_mutex);
  if (!is_finished) {
    is_finished = true;
    tsqueue->setFinished();
  }
  lock.unlock();
  tsqueue->signal();
}

void WebspiderThreadpools::join_workers(std::thread &thread, bool verbose) {
  try {
    std::thread::id tid = thread.get_id();
    thread.join();
    if (verbose) {
      std::cout << "Spider: completed join with thread " << tid << std::endl;
    }
  } catch (const std::system_error &e) {
    std::cout << "ERROR: Caught system_error with code " << e.code()
              << " meaning " << e.what() << '\n';
  }
}
