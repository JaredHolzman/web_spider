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
  std::cout << "Getting " << webpage_address << " html." << std::endl;
  get_page_html(webpage_address, &webpage_html);
  if (webpage_html.empty()) {
    std::cout << "Page blank" << std::endl;
    return std::vector<std::string *>();
  }

  std::cout << "Parsing " << webpage_address << " html." << std::endl;
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
    std::cout << "Start fetching" << std::endl;
    res = curl_easy_perform(curl);
    std::cout << "Done fetching" << std::endl;
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

std::string WebPageScraper::parse_url(std::string base, std::string href) {

  SoupURI *_base = soup_uri_new((base).c_str());
  SoupURI *_url = href.find_first_of(":") > href.find_first_of("/#?")
                      ? _url = soup_uri_new_with_base(_base, href.c_str())
                      : _url = soup_uri_new(href.c_str());

  if (!SOUP_URI_VALID_FOR_HTTP(_url)) {
    return "";
  }

  soup_uri_set_fragment(_url, NULL);
  soup_uri_set_query(_url, NULL);
  return std::string(soup_uri_to_string(_url, FALSE));
}
