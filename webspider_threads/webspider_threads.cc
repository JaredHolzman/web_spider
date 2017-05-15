#include "webspider_threads.h"

WebspiderThreads::WebspiderThreads(const std::string &root_webpage_address,
                                   const size_t max_threads,
                                   const int max_depth,
                                   ThreadsafeQueue<Page> &tsqueue,
                                   HTMLScraper &scraper, const bool verbose)
    : root_webpage_address(root_webpage_address),
      max_threads(max_threads),
      max_depth(max_depth),
      verbose(verbose),
      tsqueue(tsqueue),
      scraper(scraper),
      finished_mutex(),
      is_finished(false),
      avail_threads_cv(),
      avail_threads(max_threads) {}

WebspiderThreads::~WebspiderThreads() {}

void WebspiderThreads::crawl_web() {
  tsqueue.append(std::unique_ptr<Page>(new Page(
      std::unique_ptr<std::string>(new std::string(root_webpage_address)), 0)));

  size_t thread_number = 0;
  while (!tsqueue.isEmpty() || !is_finished) {
    std::unique_lock<std::mutex> lock(finished_mutex);
    // Max number of threads are currently running
    avail_threads_cv.wait(lock, [this] { return avail_threads != 0; });
    std::thread(&WebspiderThreads::crawl_page, this, thread_number).detach();
    thread_number++;
    avail_threads--;
    lock.unlock();
  }

  std::unique_lock<std::mutex> lock(finished_mutex);
  // While there are still threads running, wait
  avail_threads_cv.wait(lock, [this] { return avail_threads == max_threads; });
  lock.unlock();
}

void WebspiderThreads::crawl_page(size_t thread_number) {
  std::unique_ptr<Page> page = tsqueue.remove();

  if (page == NULL) {
    tsqueue.signal();
    std::unique_lock<std::mutex> lock(finished_mutex);
    avail_threads++;
    avail_threads_cv.notify_one();
    lock.unlock();
    return;
  }

  if (verbose) {
    std::cout << *(page->page_href) << " " << page->depth << std::endl;
  }

  std::vector<std::unique_ptr<std::string>> linked_pages;
  scraper.get_page_hrefs(*page->page_href, thread_number, &linked_pages);

  int depth_next = page->depth + 1;
  for (size_t i = 0; i < linked_pages.size(); i++) {
    // std::cout << *linked_pages[i] << " " << depth_next << std::endl;
    if (depth_next <= max_depth) {
      std::unique_ptr<Page> page = std::unique_ptr<Page>(
          new Page(std::move(linked_pages[i]), depth_next));
      tsqueue.append(std::move(page));
    }
  }

  std::unique_lock<std::mutex> lock(finished_mutex);
  if (depth_next > max_depth) {
    if (!is_finished) {
      is_finished = true;
      tsqueue.setFinished();
    }
    tsqueue.signal();
  }

  avail_threads++;
  avail_threads_cv.notify_one();
  lock.unlock();
}
