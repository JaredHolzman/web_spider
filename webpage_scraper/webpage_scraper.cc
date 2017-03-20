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
     Use library to parse the HTML(gumbo-query which wraps
       Google's gumbo-parser for C++),
     Hack it and search each line for all occurrences of /<a href="/ and
     grab text until closing /"/

   Depending on how messy/not messy it gets, parse_html may be broken it several
   methods that it calls.

   Returns vector of string pointers for each href found.
**/
std::vector<std::string *> WebPageScraper::parse_html(std::string page_html) {
  std::vector<std::string *> page_hrefs;
  std::vector<GumboNode *> nodes;

  // Dummy code
  page_hrefs.push_back(new std::string(page_html));
  //Test
  std::string html = "<h1>Hello, World!</h1>";
  GumboOutput *output = gumbo_parse(html.c_str());
  GumboNode *root = output->root;
  nodes.push_back(root);

  std::cout << "href->value" << std::endl;

  // while (!nodes.empty()) {
  //   GumboNode *node = nodes.back();
  //   nodes.pop_back();
  //   if (node->type != GUMBO_NODE_ELEMENT) {
  //    continue;
  //   }

  //   std::cout << nodes.size() << " : " << node->v.element.tag << std::endl;

  //   if (node->v.element.tag == GUMBO_TAG_A) {
  //     GumboAttribute *href =
  //         gumbo_get_attribute(&node->v.element.attributes, "href");
  //     if (href != NULL) {
  //       std::cout << href->value << std::endl;
  //       page_hrefs.push_back(new std::string(href->value));
  //     }
  //   }

  //   GumboVector *children = &root->v.element.children;
  //   for (size_t i = 0; i < children->length; i++) {
  //     nodes.push_back((GumboNode *)(children->data[i]));
  //   }
  // }

  gumbo_destroy_output(&kGumboDefaultOptions, output);

  return page_hrefs;
}
