#ifndef _WEBPAGE_SCRAPER_H
#define _WEBPAGE_SCRAPER_H

#include <boost/asio.hpp>
#include <vector>
#include <iostream>
#include <string>

class WebPageScraper {
public:
  WebPageScraper();
  std::vector<std::string *> get_page_hrefs(std::string page);

private:
  std::string get_page_html(std::string page);
  std::vector<std::string *> parse_html(std::string page_html);
};

#endif
