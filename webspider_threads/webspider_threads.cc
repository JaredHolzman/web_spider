#include "webspider_threads.h"

WebspiderThreads::WebspiderThreads(
    std::string root_webpage_address, size_t max_threads, int max_depth,
    std::unique_ptr<ThreadsafeQueue<Page>> tsqueue,
    std::unique_ptr<HTMLScraper> scraper)
    : root_webpage_address(root_webpage_address), max_threads(max_threads),
      max_depth(max_depth), tsqueue(std::move(tsqueue)),
      scraper(std::move(scraper)), finished_mutex(), is_finished(false),
      avail_threads_cv(), avail_threads(max_threads) {}

WebspiderThreads::~WebspiderThreads() {}

void WebspiderThreads::crawl_web() {
  tsqueue->append(new Page(&root_webpage_address, new std::string("ROOT"), 0));

  while (!tsqueue->isEmpty() || !is_finished) {
    std::unique_lock<std::mutex> lock(finished_mutex);
    // Max number of threads are currently running
    avail_threads_cv.wait(lock, [this] { return avail_threads != 0; });
    std::thread(&WebspiderThreads::crawl_page, this).detach();
    avail_threads--;
    lock.unlock();
  }

  std::unique_lock<std::mutex> lock(finished_mutex);
  // While there are still threads running, wait
  avail_threads_cv.wait(lock, [this] { return avail_threads == max_threads; });
  lock.unlock();
}

void WebspiderThreads::crawl_page() {
  Page *page = tsqueue->remove();

  if (page == NULL) {
    tsqueue->signal();
    std::unique_lock<std::mutex> lock(finished_mutex);
    avail_threads++;
    avail_threads_cv.notify_one();
    lock.unlock();
    return;
  }

  std::vector<std::string *> linked_pages =
      scraper->get_page_hrefs(*page->page_href);

  int depth_next = page->depth + 1;
  for (size_t i = 0; i < linked_pages.size(); i++) {
    // std::cout << *linked_pages[i] << " " << depth_next << std::endl;
    if (depth_next < max_depth) {
      tsqueue->append(new Page(linked_pages[i], page->page_href, depth_next));
    }
  }

  std::unique_lock<std::mutex> lock(finished_mutex);
  if (depth_next >= max_depth) {
    if (!is_finished) {
      is_finished = true;
      tsqueue->setFinished();
    }
    tsqueue->signal();
  }

  avail_threads++;
  avail_threads_cv.notify_one();
  lock.unlock();
}
