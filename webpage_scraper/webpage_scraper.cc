#include "webpage_scraper.h"

WebPageScraper::WebPageScraper() {}

/**
   Takes in webpage address and returns a vector of string pointers of all
   hrefs on that page.
 **/
std::vector<std::string *> WebPageScraper::get_page_hrefs(std::string page) {
  std::string page_html = get_page_html(page);
  std::vector<std::string *> page_hrefs = parse_html(page_html);

  return page_hrefs;
}

/**
   Modified from https://www.rosettacode.org/wiki/Web_scraping#C.2B.2B

   Takes in a webpage address and returns a string of all HTML for that page.
   Includes response headers as well for now for debugging.
**/
std::string WebPageScraper::get_page_html(std::string page) {
  std::string page_text = "";
  boost::asio::ip::tcp::iostream s(page, "http");
  if (!s)
    std::cout << "Could not connect to " << page;
  s << "GET / HTTP/1.0\r\n"
    << "Host: " << page << "\r\n"
    << "Accept: */*\r\n"
    << "Connection: close\r\n\r\n";
  for (std::string line; getline(s, line);) {
    page_text.append(line);
    page_text.append("\n");
  }
  return page_text;
}

/**
   Takes in a string of HTML and parses it, looking for all hrefs on the page.
   TODO: couple different implementation options:
     Convert to XML and parse that (libtidy, libxml),
     Find library to just parse the HTML directly (gumbo-query which wraps
       Google's gumbo-parser for C++),
     Hack it and search each line for all occurrences of /<a href="/ and
     grab text until closing /"/

   Depending on how messy/not messy it gets, parse_html may be broken it several
   methods that it calls.

   Returns vector of string pointers for each href found.
**/
std::vector<std::string *> WebPageScraper::parse_html(std::string page_html) {
  std::vector<std::string *> page_hrefs;

  // Dummy code
  page_hrefs.push_back(new std::string(page_html));
  return page_hrefs;
}
