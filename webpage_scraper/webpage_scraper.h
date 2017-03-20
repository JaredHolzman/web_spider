#ifndef _WEBPAGE_SCRAPER_H
#define _WEBPAGE_SCRAPER_H

#include <boost/asio.hpp>
#include "gumbo.h"
#include <vector>
#include <iostream>
#include <string>

class WebPageScraper {
public:
  WebPageScraper();
  std::vector<std::string *> get_page_hrefs(std::string webpage_address);

private:
  std::string get_page_html(std::string webpage_address);
  std::vector<std::string *> parse_html(std::string webpage_html, std::string webpage_address);
};

#endif
