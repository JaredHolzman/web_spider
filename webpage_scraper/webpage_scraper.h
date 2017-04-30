#ifndef _WEBPAGE_SCRAPER_H
#define _WEBPAGE_SCRAPER_H

#include <Poco/URI.h>
#include <curl/curl.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "gumbo.h"

size_t curl_to_string(void *ptr, size_t size, size_t nmemb, void *data);

class HTMLScraper {
 public:
  HTMLScraper(const std::string &root_url);
  HTMLScraper(const std::string &root_url,
                   const std::string &log_file_name);
  ~HTMLScraper();
  void get_page_hrefs(const std::string &webpage_address,
                      std::vector<std::unique_ptr<std::string>> *hrefs);

 private:
  std::ofstream log_file;
  std::string root_url_host;
  void get_page_html(const std::string &webpage_address,
                     std::string *webpage_html);
  void parse_html(const std::string &webpage_html,
                  const std::string &webpage_address,
                  std::vector<std::unique_ptr<std::string>> *hrefs);
  void parse_url(const std::string &_base, const std::string &_href,
                 std::string *parsed_url);
};

#endif
