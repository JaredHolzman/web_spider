#ifndef _PAGE_H
#define _PAGE_H

#include <memory>
#include <string>

struct Page {
  const std::unique_ptr<std::string> page_href;
  const int depth;
  bool operator==(const Page &other) const {
    return *page_href == *(other.page_href);
  }

  Page(std::unique_ptr<std::string> page_href, int depth)
      : page_href(std::move(page_href)), depth(depth) {}

  Page(const Page &page): page_href(std::unique_ptr<std::string>(new std::string(*page.page_href))), depth(page.depth) {}
  ~Page() {}
};

namespace std {
template <>
struct hash<Page> {
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
