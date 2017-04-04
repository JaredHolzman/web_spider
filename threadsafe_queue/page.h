#ifndef _PAGE_H
#define _PAGE_H

#include <string>

struct Page {
  std::string *page_href;
  std::string *parent;
  int depth;
  bool operator==(const Page &other) const {
    return *page_href == *(other.page_href);
  }
  Page(std::string *page_href, std::string *parent, int depth)
    : page_href(page_href), parent(parent), depth(depth) {}
};

namespace std {
  template <> struct hash<Page> {
    std::size_t operator()(const Page &p) const {
      using std::size_t;
      using std::hash;
      using std::string;

      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:

      return hash<string>()(*p.page_href);
    }
  };
}

#endif

