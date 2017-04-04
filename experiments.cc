#include "threadsafe_queue/threadsafe_exqueue.h"
#include "threadsafe_queue/threadsafe_queue.h"
#include "webpage_scraper/webpage_scraper.h"
#include "webspider_threadpools/webspider_threadpools.h"
#include "webspider_threads/webspider_threads.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string.h>
#include <string>
#include <thread>

// Provided courtesy of Kyle Vedder of UMass' AMRL lab
double GetMonotonicTime() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  const double time = static_cast<double>(ts.tv_sec) +
                      static_cast<double>(ts.tv_nsec) * (1.0E-9);
  return time;
}

int main() {
  std::ofstream output;
  output.open("output.txt", std::ios::app);

  for (size_t max_threads = 50; max_threads < 700; max_threads += 50) {

    output << max_threads;

    for (size_t trials = 0; trials < 10; trials++) {
      double delta = 0.0;
      WebspiderThreadpools t(
          std::string("www.umass.edu"), max_threads, 4,
          std::unique_ptr<ThreadsafeExQueue<Page>>(
              new ThreadsafeExQueue<Page>()),
          std::unique_ptr<WebPageScraper>(new WebPageScraper()));

      double start_time = GetMonotonicTime();
      t.crawl_web();
      double end_time = GetMonotonicTime();
      delta += end_time - start_time;

      std::cout << "Threads:" << max_threads << " Trial: " << trials
                << std::endl;
      // Write delta time to file
      output << " " << delta;

      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    output << std::endl;
  }
  output.close();
}
