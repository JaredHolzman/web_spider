#include "threadsafe_queue/threadsafe_exqueue.h"
#include "threadsafe_queue/threadsafe_queue.h"
#include "webpage_scraper/webpage_scraper.h"
#include "webspider_threadpools/webspider_threadpools.h"
#include "webspider_threads/webspider_threads.h"

#include <string.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

double GetMonotonicTime() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  const double time = static_cast<double>(ts.tv_sec) +
                      static_cast<double>(ts.tv_nsec) * (1.0E-9);
  return time;
}

int main(int argc, char *argv[]) {
  if (argc != 2 && argc != 3) {
    std::cout << "Usage: ./experiements [http://www.example.com] [output file]"
              << std::endl;

    return 0;
  }

  std::ofstream output;
  output.open("./logs/output.txt", std::ios::app);
  std::ofstream trial_count;

  if (argc == 3) {
    trial_count.open(std::string(argv[2]), std::ios::app);
  }

  for (size_t max_threads = 1; max_threads <= 50; max_threads++) {
    output << max_threads;

    for (size_t trials = 0; trials < 20; trials++) {
      int count = 0;
      bool no_fail = true;
      double delta = 0.0;
      while (count == 0 && !no_fail) {
        std::chrono::time_point<std::chrono::system_clock> curr_time;
        curr_time = std::chrono::system_clock::now();
        std::time_t curr_timestamp =
            std::chrono::system_clock::to_time_t(curr_time);
        char *time = std::ctime(&curr_timestamp);
        time[strlen(time) - 2] = '\0';
        if (trial_count.is_open()) {
          trial_count << time << " Threads:" << max_threads
                      << " Trial: " << trials << " " << time << std::endl;
        }
        std::cout << time << " Threads:" << max_threads << " Trial: " << trials
                  << " " << std::endl;

        const std::string &curl_log = "./logs/curl_log_" +
                                      std::to_string(max_threads) + "_" +
                                      std::to_string(trials)+ "_" + std::to_string(count);
        double delta = 0.0;
        std::string root_webpage_address = std::string(argv[1]);
        ThreadsafeExQueue<Page> ts_queue;
        HTMLScraper scraper(root_webpage_address, curl_log);
        WebspiderThreadpools spider(root_webpage_address, max_threads, 4,
                                    ts_queue, scraper, false);

        double start_time = GetMonotonicTime();
        spider.crawl_web();
        double end_time = GetMonotonicTime();
        delta = end_time - start_time;

        std::ifstream trial_log;
        trial_log.open("./logs/output.txt");

        std::string line;
        while (getline(trial_log, line)) {
          no_fail =
              line.find("curl_easy_perform() failed", 0) != std::string::npos;
        }

        std::this_thread::sleep_for(std::chrono::seconds(2));

        count++;
      }
      // Write delta time to file
      output << " " << delta;

      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    output << std::endl;
  }
  output.close();
}
