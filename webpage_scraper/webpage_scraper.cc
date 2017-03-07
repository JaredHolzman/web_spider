/**
   Modified from https://www.rosettacode.org/wiki/Web_scraping#C.2B.2B
 **/

#include "webpage_scraper.h"

int main(int argc, char *argv[]) {
  boost::asio::ip::tcp::iostream s(argv[1], "http");
  if (!s)
    std::cout << "Could not connect to " << argv[1];
  s << "GET / HTTP/1.0\r\n"
    << "Host: " << argv[1] << "\r\n"
    << "Accept: */*\r\n"
    << "Connection: close\r\n\r\n";
  for (std::string line; getline(s, line);) {
    std::cout << line << std::endl;
  }
}
