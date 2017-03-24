#include "webpage_scraper.h"

// Write cURL output to string. cURL expects method with this signature.
size_t curl_to_string(void *ptr, size_t size, size_t nmemb, void *data) {
  std::string *str = static_cast<std::string *>(data);
  char *sptr = static_cast<char *>(ptr);
  for (size_t i = 0; i < size * nmemb; i++) {
    str->push_back(sptr[i]);
  }
  return size * nmemb;
}

WebPageScraper::WebPageScraper() {}

/**
   Takes in webpage address and returns a vector of string pointers of all
   hrefs on that page.
 **/
std::vector<std::string *>
WebPageScraper::get_page_hrefs(std::string webpage_address) {
  std::string webpage_html;
  get_page_html(webpage_address, &webpage_html);
  std::vector<std::string *> page_hrefs =
      parse_html(webpage_html, webpage_address);

  return page_hrefs;
}

/**
   Takes in a webpage address and returns a string of all HTML for that page.
   Includes response headers as well for now for debugging.
**/
void WebPageScraper::get_page_html(std::string webpage_address,
                                   std::string *webpage_html) {
  CURL *curl;
  CURLcode res;
  std::string pagedata;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, webpage_address.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_to_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, webpage_html);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK)
      std::wcerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                 << " " << webpage_address.c_str() << std::endl;

    /* always cleanup */
    curl_easy_cleanup(curl);
  }
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

      std::cout << parse_url(webpage_address, href_string) << std::endl;
      parse_url(webpage_address, href_string);

      size_t pos;
      // Remove route/query and trailing slash
      if ((pos = href_string.find_first_of("#?")) != std::string::npos) {
        href_string.erase(pos, std::string::npos);
      }
      if (href_string.back() == '/') {
        href_string.erase(href_string.end() - 1);
      }

      // Ignore hrefs that are not valid links to other pages
      if (href_string.front() != '/' &&
          href_string.substr(0, 4).compare("http") != 0) {
        continue;
      }

      if ((pos = href_string.find("://")) != std::string::npos) {
        page_hrefs.push_back(
            new std::string(href_string.substr(pos + 3, std::string::npos)));
      } else {
        page_hrefs.push_back(new std::string(
            webpage_address.substr(0, webpage_address.find_first_of("/")) +
            href_string));
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

std::string WebPageScraper::parse_url(std::string base, std::string href) {
  if (base.empty() || href.empty()) {
    std::cout << "ALKJFSALKJFLKJDSALKJFDSALKJFLKSADLKJFSALKJDFLKJSALKFLKSAFLKA"
              << std::endl;
    return "";
  }

  SoupURI *_base = soup_uri_new(("http://" + base).c_str());
  if (!SOUP_URI_IS_VALID(_base)) {
    std::cout << "BAAAAAD: " << base << std::endl;
  }
  SoupURI *_url = href.find_first_of(":") < href.find_first_of("/#?")
                      ? soup_uri_new(href.c_str())
                      : soup_uri_new_with_base(_base, href.c_str());

  // soup_uri_set_fragment(_url, "");
  if (_url == NULL) {
    std::cout << base << " -> " << href << base << std::endl;
    // return "";
  }

  return std::string(soup_uri_to_string(_url, FALSE));
}
