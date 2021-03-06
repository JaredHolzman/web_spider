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

std::string make_directories(size_t max_threads, size_t trials) {
  // Make path '/logs/max_threads/trial'
  std::string curl_log_path =
      "./logs/" + std::to_string(max_threads) + "/" + std::to_string(trials);
  system(("mkdir -p " + curl_log_path).c_str());

  // Make sub directory for each thread
  for (size_t i = 0; i < max_threads; i++) {
    system(("mkdir " + curl_log_path + "/" + std::to_string(i)).c_str());
  }

  return curl_log_path;
}

int main(int argc, char *argv[]) {
  if (argc != 2 && argc != 3) {
    std::cout << "Usage: ./experiments [http://www.example.com] [output file]"
              << std::endl;

    return 0;
  }

  system("mkdir -p ./logs/");
  std::ofstream data;
  data.open("./logs/data.csv", std::ios::app);
  std::ofstream trial_count;

  if (argc == 3) {
    trial_count.open(std::string(argv[2]), std::ios::app);
  }

  for (size_t max_threads = 1; max_threads <= 50; max_threads++) {
    data << max_threads;

    for (size_t trials = 0; trials < 20; trials++) {
      int count = 0;
      bool no_fail = false;
      double delta = 0.0;

      // Will repeat each trial up to 10 times or until there is no error
      while (!no_fail && count < 10) {
        std::chrono::time_point<std::chrono::system_clock> curr_time;
        curr_time = std::chrono::system_clock::now();
        std::time_t curr_timestamp =
            std::chrono::system_clock::to_time_t(curr_time);
        char *time = std::ctime(&curr_timestamp);
        time[strlen(time) - 2] = '\0';
        if (trial_count.is_open()) {
          trial_count << time << " Threads: " << max_threads
                      << " Trial: " << trials << " " << std::endl;
        }
        std::cout << time << " Threads: " << max_threads << " Trial: " << trials
                  << std::endl;

        std::string curl_log_path = make_directories(max_threads, trials);
        const std::string curl_log = "curl_log_" + std::to_string(max_threads) +
                                     "_" + std::to_string(trials) + "_" +
                                     std::to_string(count);

        std::string root_webpage_address = std::string(argv[1]);
        ThreadsafeExQueue<Page> ts_queue;
        HTMLScraper scraper(root_webpage_address, curl_log_path, curl_log,
                            curl_log + "_errors");
        WebspiderThreadpools spider(root_webpage_address, max_threads, 3,
                                    ts_queue, scraper, false);

        double start_time = GetMonotonicTime();
        spider.crawl_web();
        double end_time = GetMonotonicTime();
        delta = end_time - start_time;

        // Check if any errors occured
        no_fail = scraper.get_curl_failures() == 0;

        std::this_thread::sleep_for(std::chrono::seconds(2));
        count++;
      }
      // Write delta time to file
      data << ", " << delta;

      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    data << std::endl;
  }
  data.close();
}
