#include "webpage_scraper/webpage_scraper.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  WebPageScraper scraper = WebPageScraper();
  // Not actually the hrefs for now, just HTML blob
  std::vector<std::string *> hrefs = scraper.get_page_hrefs(argv[1]);

  for(std::string *s: hrefs){
    std::cout << *s;
  }
}
