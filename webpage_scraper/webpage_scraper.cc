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

HTMLScraper::HTMLScraper(const std::string &root_url) {
  Poco::URI root_uri(root_url);
  root_url_host = root_uri.getHost();
  curl_global_init(CURL_GLOBAL_ALL);
}
HTMLScraper::HTMLScraper(const std::string &root_url,
                         const std::string &log_file_name)
    : log_file() {
  Poco::URI root_uri(root_url);
  root_url_host = root_uri.getHost();

  curl_global_init(CURL_GLOBAL_ALL);

  log_file.open(log_file_name, std::ios::app);
}
HTMLScraper::~HTMLScraper() {
  curl_global_cleanup();
  log_file.close();
}

/**
   Takes in webpage address and a vector of string pointers and fills the vector
 with of all hrefs on that page.
 **/
void HTMLScraper::get_page_hrefs(
    const std::string &webpage_address,
    std::vector<std::unique_ptr<std::string>> *hrefs) {
  std::string webpage_html;
  get_page_html(webpage_address, &webpage_html);

  if (webpage_html.empty()) {
    return;
  }

  parse_html(webpage_html, webpage_address, hrefs);

  // Write to log file if one was specified
  if (log_file.is_open()) {
    std::chrono::time_point<std::chrono::system_clock> curr_time;
    curr_time = std::chrono::system_clock::now();
    std::time_t curr_timestamp =
        std::chrono::system_clock::to_time_t(curr_time);
    log_file << webpage_address << " " << std::ctime(&curr_timestamp)
             << std::endl;
  }
}

/**
   Takes in a webpage address and returns a string of all HTML for that page.
   Includes response headers as well for now for debugging.
**/
void HTMLScraper::get_page_html(const std::string &webpage_address,
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
    // curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 0);

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
void HTMLScraper::parse_html(const std::string &webpage_html,
                             const std::string &webpage_address,
                             std::vector<std::unique_ptr<std::string>> *hrefs) {
  std::vector<GumboNode *> nodes;

  GumboOutput *output = gumbo_parse(webpage_html.c_str());
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
      std::string parsed_url;
      parse_url(webpage_address, href_string, &parsed_url);
      if (!parsed_url.empty()) {
        hrefs->push_back(
            std::unique_ptr<std::string>(new std::string(parsed_url)));
      }
    }

    GumboVector *children = &node->v.element.children;
    for (size_t i = 0; i < children->length; i++) {
      nodes.push_back((GumboNode *)(children->data[i]));
    }
  }

  gumbo_destroy_output(&kGumboDefaultOptions, output);
}

void HTMLScraper::parse_url(const std::string &_base, const std::string &_href,
                            std::string *parsed_url) {
  Poco::URI base;
  Poco::URI url;

  try {
    base = Poco::URI(_base);
  } catch (const std::exception &e) {
    std::chrono::time_point<std::chrono::system_clock> curr_time;
    curr_time = std::chrono::system_clock::now();
    std::time_t curr_timestamp =
        std::chrono::system_clock::to_time_t(curr_time);
    if (log_file.is_open()) {
      log_file << e.what()
               << ": Poco::Syntax exception. Failed to parse base: " << _base
               << " " << std::ctime(&curr_timestamp) << std::endl;
    } else {
      std::cerr << e.what()
                << ": Poco::Syntax exception. Failed to parse base: " << _base
                << " " << std::ctime(&curr_timestamp) << std::endl;
    }
    *parsed_url = "";
    return;
  }
  try {
    url = Poco::URI(base, _href);
  } catch (const std::exception &e) {
    std::chrono::time_point<std::chrono::system_clock> curr_time;
    curr_time = std::chrono::system_clock::now();
    std::time_t curr_timestamp =
        std::chrono::system_clock::to_time_t(curr_time);
    if (log_file.is_open()) {
      log_file << e.what()
               << ": Poco::Syntax exception. Failed to parse href: " << _href
               << " " << std::ctime(&curr_timestamp) << std::endl;
    } else {
      std::cerr << e.what()
                << ": Poco::Syntax exception. Failed to parse href: " << _href
                << " " << std::ctime(&curr_timestamp) << std::endl;
    }
    *parsed_url = "";
    return;
  }

  url.setFragment("");
  url.setQuery("");

  if (url.getHost() != root_url_host ||
      (url.getPath().length() >= 4 &&
       url.getPath().substr(url.getPath().length() - 4, std::string::npos) ==
           ".pdf")) {
    *parsed_url = "";
    return;
  }

  //   std::cout << url.toString() << std::endl;
  *parsed_url = url.toString();
}
