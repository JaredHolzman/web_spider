#include "webpage_scraper.h"

WebPageScraper::WebPageScraper() {}

/**
   Takes in webpage address and returns a vector of string pointers of all
   hrefs on that page.
 **/
std::vector<std::string *>
WebPageScraper::get_page_hrefs(std::string webpage_address) {
  std::string webpage_html = get_page_html(webpage_address);
  std::vector<std::string *> page_hrefs =
      parse_html(webpage_html, webpage_address);

  return page_hrefs;
}

/**
   Modified from https://www.rosettacode.org/wiki/Web_scraping#C.2B.2B

   Takes in a webpage address and returns a string of all HTML for that page.
   Includes response headers as well for now for debugging.
**/
std::string WebPageScraper::get_page_html(std::string webpage_address) {
  std::string page_text = "";
  boost::asio::ip::tcp::iostream s(webpage_address, "http");
  if (!s)
    std::cout << "Could not connect to " << webpage_address;
  s << "GET / HTTP/1.0\r\n"
    << "Host: " << webpage_address << "\r\n"
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
   Uses Google's HTML gumbo-parser library.
   Returns vector of string pointers for each href found.
**/
std::vector<std::string *>
WebPageScraper::parse_html(std::string page_html, std::string webpage_address) {
  std::vector<std::string *> page_hrefs;
  std::vector<GumboNode *> nodes;

  GumboOutput *output = gumbo_parse(page_html.c_str());
  GumboNode *root = output->root;
  nodes.push_back(root);

  while (!nodes.empty()) {
    GumboNode *node = nodes.back();
    nodes.pop_back();
    if (node->type != GUMBO_NODE_ELEMENT) {
      continue;
    }

    GumboAttribute *href;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href"))) {
      std::string href_string = std::string(href->value);
      size_t pos;
      if ((pos = href_string.find_first_of("://"))) {
        page_hrefs.push_back(
            new std::string(href_string.substr(pos + 3, std::string::npos)));
      } else {
        page_hrefs.push_back(
            new std::string(webpage_address.append(href_string)));
      }
    }

    GumboVector *children = &node->v.element.children;
    for (size_t i = 0; i < children->length; i++) {
      nodes.push_back((GumboNode *)(children->data[i]));
    }
  }

  gumbo_destroy_output(&kGumboDefaultOptions, output);
  return page_hrefs;
}
