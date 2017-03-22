#ifndef _WEBPAGE_SCRAPER_H
#define _WEBPAGE_SCRAPER_H

#include "gumbo.h"
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>

size_t curl_to_string(void *ptr, size_t size, size_t nmemb, void *data);

class WebPageScraper {
public:
  WebPageScraper();
  std::vector<std::string *> get_page_hrefs(std::string webpage_address);

private:
  void get_page_html(std::string webpage_address, std::string *webpage_html);
  std::vector<std::string *> parse_html(std::string webpage_html,
                                        std::string webpage_address);
};

#endif
