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

HTMLScraper::HTMLScraper() {}
HTMLScraper::HTMLScraper(const std::string &log_file_name) : log_file() {
  log_file.open(log_file_name, std::ios::app);
}
HTMLScraper::~HTMLScraper() { log_file.close(); }

/**
   Takes in webpage address and returns a vector of string pointers of all
   hrefs on that page.
 **/
std::vector<std::string *>
HTMLScraper::get_page_hrefs(std::string webpage_address) {
  std::string webpage_html;
  get_page_html(webpage_address, &webpage_html);
  if (webpage_html.empty()) {
    return std::vector<std::string *>();
  }

  std::vector<std::string *> page_hrefs =
      parse_html(webpage_html, webpage_address);

  return page_hrefs;
}

/**
   Takes in a webpage address and returns a string of all HTML for that page.
   Includes response headers as well for now for debugging.
**/
void HTMLScraper::get_page_html(std::string webpage_address,
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
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, -1L);

    res = curl_easy_perform(curl);
    /* Check for errors */
    if (res != CURLE_OK) {
      std::chrono::time_point<std::chrono::system_clock> curr_time;
      curr_time = std::chrono::system_clock::now();
      std::time_t curr_timestamp =
          std::chrono::system_clock::to_time_t(curr_time);
      if (log_file.is_open()) {
        log_file << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                 << " " << webpage_address.c_str() << " "
                 << std::ctime(&curr_timestamp) << std::endl;
      } else {
        std::wcerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                   << " " << webpage_address.c_str() << " "
                   << std::ctime(&curr_timestamp) << std::endl;
      }
    }

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
HTMLScraper::parse_html(std::string page_html, std::string webpage_address) {
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
    std::string href_string;
    if (node->v.element.tag == GUMBO_TAG_A &&
        (href = gumbo_get_attribute(&node->v.element.attributes, "href")) &&
        !(href_string = std::string(href->value)).empty()) {
      std::string parsed_href_string = parse_url(webpage_address, href_string);
      if (!parsed_href_string.empty()) {
        page_hrefs.push_back(new std::string(parsed_href_string));
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

std::string HTMLScraper::parse_url(std::string base, std::string href) {

  SoupURI *_base = soup_uri_new((base).c_str());
  SoupURI *_url = href.find_first_of(":") > href.find_first_of("/#?")
                      ? _url = soup_uri_new_with_base(_base, href.c_str())
                      : _url = soup_uri_new(href.c_str());

  if (!SOUP_URI_VALID_FOR_HTTP(_url)) {
    return "";
  }

  // Filter out non-umass urls, hack
  std::string host = std::string(soup_uri_get_host(_url));
  if (host != "umass.edu" && host != "www.umass.edu") {
    return "";
  }

  soup_uri_set_fragment(_url, NULL);
  soup_uri_set_query(_url, NULL);
  std::string url(soup_uri_to_string(_url, FALSE));
  soup_uri_free(_url);

  if (url.substr(url.length() - 4, std::string::npos) == ".pdf") {
    return "";
  }
  return url;
}
