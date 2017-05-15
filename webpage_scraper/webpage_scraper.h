#ifndef _WEBPAGE_SCRAPER_H
#define _WEBPAGE_SCRAPER_H

#include <Poco/URI.h>
#include <curl/curl.h>
#include <string.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include "gumbo.h"

size_t curl_to_string(void *ptr, size_t size, size_t nmemb, void *data);

class HTMLScraper {
 public:
  HTMLScraper(const std::string &root_url);
  HTMLScraper(const std::string &root_url, const std::string &path_to_logs,
              const std::string &log_file_name,
              const std::string &error_log_file_name);
  ~HTMLScraper();
  void get_page_hrefs(const std::string &webpage_address,
                      const size_t &thread_number,
                      std::vector<std::unique_ptr<std::string>> *hrefs);
int get_curl_failures();
 private:
  std::string path_to_logs;
  std::string log_file_name;
  std::string error_log_file_name;
  std::string root_url_host;
  std::mutex failures_mutex;
  int curl_failures;
  
  void write_log(const CURLcode &res, const std::string &webpage_address,
                 const double &namelookup_time, const double &connect_time,
                 const double &appconnect_time, const double &pretransfer_time,
                 const double &redirect_time, const double &starttransfer_time,
                 const double &total_time, std::ofstream &log_file, std::ofstream &error_log_file);

  void get_page_html(const std::string &webpage_address, 
                     std::string *webpage_html, std::ofstream &log_file, std::ofstream &error_log_file);
  void parse_html(const std::string &webpage_html,
                  const std::string &webpage_address,
                  std::vector<std::unique_ptr<std::string>> *hrefs, std::ofstream &log_file, std::ofstream &error_log_file);
  void parse_url(const std::string &_base, const std::string &_href,
                 std::string *parsed_url, std::ofstream &log_file, std::ofstream &error_log_file);
};

#endif
