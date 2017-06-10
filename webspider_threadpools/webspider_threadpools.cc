#include "webspider_threadpools.h"

WebspiderThreadpools::WebspiderThreadpools(
    const std::string &root_webpage_address, const size_t max_threads,
    const int max_depth, ThreadsafeQueue<Page> &tsqueue, HTMLScraper &scraper,
    const bool verbose)
    : root_webpage_address(root_webpage_address),
      max_threads(max_threads),
      max_depth(max_depth),
      verbose(verbose),
      tsqueue(tsqueue),
      scraper(scraper),
      finished_mutex(),
      is_finished(false) {}

WebspiderThreadpools::~WebspiderThreadpools() {}

void WebspiderThreadpools::crawl_web() {
  std::thread workers[max_threads];

  tsqueue.append(std::unique_ptr<Page>(new Page(
      std::unique_ptr<std::string>(new std::string(root_webpage_address)), 0)));

  // Create threads to crawl webpages
  for (size_t i = 0; i < max_threads; i++) {

    workers[i] = std::thread(&WebspiderThreadpools::crawl_page, this, i);
  }

  // Wait for threads to complete
  for (size_t i = 0; i < max_threads; i++) {
    WebspiderThreadpools::join_workers(workers[i], false);
  }
}

void WebspiderThreadpools::crawl_page(size_t thread_number) {
  while (!is_finished) {
    std::unique_ptr<Page> page = tsqueue.remove();

    if (page == NULL) {
      break;
    }

    if (verbose) {
      std::cout << *(page->page_href) << " " << page->depth << std::endl;
    }

    std::vector<std::unique_ptr<std::string>> linked_pages;
    scraper.get_page_hrefs(*page->page_href, thread_number, &linked_pages);

    int depth_next = page->depth + 1;
    for (size_t i = 0; i < linked_pages.size(); i++) {
      if (depth_next <= max_depth) {
        std::unique_ptr<Page> page = std::unique_ptr<Page>(
            new Page(std::move(linked_pages[i]), depth_next));
        tsqueue.append(std::move(page));
      }
    }

    if (depth_next > max_depth) {
      break;
    }
  }

  std::unique_lock<std::mutex> lock(finished_mutex);
  if (!is_finished) {
    is_finished = true;
    tsqueue.setFinished();
  }
  lock.unlock();
  tsqueue.signal();
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
