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
                         const std::string &path_to_logs,
                         const std::string &log_file_name,
                         const std::string &error_log_file_name)
    : path_to_logs(path_to_logs),
      log_file_name(log_file_name),
      error_log_file_name(error_log_file_name),
      failures_mutex(),
      curl_failures(0) {
  Poco::URI root_uri(root_url);
  root_url_host = root_uri.getHost();
  curl_global_init(CURL_GLOBAL_ALL);
}
HTMLScraper::~HTMLScraper() { curl_global_cleanup(); }

int HTMLScraper::get_curl_failures() {
  std::unique_lock<std::mutex> lock(failures_mutex);
  return curl_failures;
}

/**
   Takes in webpage address and a vector of string pointers and fills the vector
 with of all hrefs on that page.
 **/
void HTMLScraper::get_page_hrefs(
    const std::string &webpage_address, const size_t &thread_number,
    std::vector<std::unique_ptr<std::string>> *hrefs) {
  std::ofstream log_file;
  std::ofstream error_log_file;
  log_file.open(
      path_to_logs + "/" + std::to_string(thread_number) + "/" + log_file_name,
      std::ios::app);
  error_log_file.open(path_to_logs + "/" + std::to_string(thread_number) + "/" +
                          error_log_file_name,
                      std::ios::app);

  std::string webpage_html;
  get_page_html(webpage_address, &webpage_html, log_file, error_log_file);

  if (webpage_html.empty()) {
    return;
  }

  parse_html(webpage_html, webpage_address, hrefs, log_file, error_log_file);

  log_file.close();
  error_log_file.close();
}

/**
 * timestamp, webpage_address, cURL_code, namelookup_time,
 * connect_time, appconnect_time, pretransfer_time, redirect_time,
 * starttransfer_time, total_time
**/
void HTMLScraper::write_log(
    const CURLcode &res, const std::string &webpage_address,
    const double &namelookup_time, const double &connect_time,
    const double &appconnect_time, const double &pretransfer_time,
    const double &redirect_time, const double &starttransfer_time,
    const double &total_time, std::ofstream &log_file,
    std::ofstream &error_log_file) {
  std::chrono::time_point<std::chrono::system_clock> curr_time;
  curr_time = std::chrono::system_clock::now();
  std::time_t curr_timestamp = std::chrono::system_clock::to_time_t(curr_time);
  char *ctime = std::ctime(&curr_timestamp);
  ctime[strlen(ctime) - 2] = '\0';

  if (!log_file.is_open()) {
    if (res != CURLE_OK) {
      std::wcerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                 << " " << webpage_address.c_str() << " " << ctime << std::endl;
    }
    return;
  }

  log_file << ctime << ", " << webpage_address << ", "
           << curl_easy_strerror(res) << ", " << namelookup_time << ", "
           << connect_time << ", " << appconnect_time << ", "
           << pretransfer_time << ", " << redirect_time << ", "
           << starttransfer_time << ", " << total_time << std::endl;

  if (res != CURLE_OK && error_log_file.is_open()) {
    error_log_file << ctime
                   << " curl_easy_perform() failed: " << curl_easy_strerror(res)
                   << " " << webpage_address << std::endl;
    std::unique_lock<std::mutex> lock(failures_mutex);
    curl_failures++;
    lock.unlock();
  }
}

/**
   Takes in a webpage address and returns a string of all HTML for that page.
   Includes response headers as well for now for debugging.
**/
void HTMLScraper::get_page_html(const std::string &webpage_address,
                                std::string *webpage_html,
                                std::ofstream &log_file,
                                std::ofstream &error_log_file) {
  CURL *curl;
  CURLcode res;
  std::string pagedata;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, webpage_address.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_to_string);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, webpage_html);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 0);

    res = curl_easy_perform(curl);

    // Get latency data
    double namelookup_time;
    curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &namelookup_time);
    double connect_time;
    curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &connect_time);
    double appconnect_time;
    curl_easy_getinfo(curl, CURLINFO_APPCONNECT_TIME, &appconnect_time);
    double pretransfer_time;
    curl_easy_getinfo(curl, CURLINFO_PRETRANSFER_TIME, &pretransfer_time);
    double redirect_time;
    curl_easy_getinfo(curl, CURLINFO_REDIRECT_TIME, &redirect_time);
    double starttransfer_time;
    curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &starttransfer_time);
    double total_time;
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &total_time);

    write_log(res, webpage_address, namelookup_time, connect_time,
              appconnect_time, pretransfer_time, redirect_time,
              starttransfer_time, total_time, log_file, error_log_file);

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
                             std::vector<std::unique_ptr<std::string>> *hrefs,
                             std::ofstream &log_file,
                             std::ofstream &error_log_file) {
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
      parse_url(webpage_address, href_string, &parsed_url, log_file,
                error_log_file);
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
                            std::string *parsed_url, std::ofstream &log_file,
                            std::ofstream &error_log_file) {
  Poco::URI base;
  Poco::URI url;

  try {
    base = Poco::URI(_base);
  } catch (const std::exception &e) {
    std::chrono::time_point<std::chrono::system_clock> curr_time;
    curr_time = std::chrono::system_clock::now();
    std::time_t curr_timestamp =
        std::chrono::system_clock::to_time_t(curr_time);
    char *ctime = std::ctime(&curr_timestamp);
    ctime[strlen(ctime) - 2] = '\0';
    if (error_log_file.is_open()) {
      error_log_file << ctime << " " << e.what()
                     << ": Poco::Syntax exception. Failed to parse base: "
                     << _base << std::endl;
    } else {
      std::cerr << ctime << " " << e.what()
                << ": Poco::Syntax exception. Failed to parse base: " << _base
                << std::endl;
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
    char *ctime = std::ctime(&curr_timestamp);
    ctime[strlen(ctime) - 2] = '\0';
    if (error_log_file.is_open()) {
      error_log_file << ctime << " " << e.what()
                     << ": Poco::Syntax exception. Failed to parse href: "
                     << _href << std::endl;
    } else {
      std::cerr << ctime << " " << e.what()
                << ": Poco::Syntax exception. Failed to parse href: " << _href
                << std::endl;
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

  // std::cout << url.toString() << std::endl;
  *parsed_url = url.toString();
}
