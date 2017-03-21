#ifndef _PAGE_H
#define _PAGE_H

#include <string>

class Page {
public:
  Page(std::string *page_href, int depth);
  std::string * get_href();
  int get_depth();

private:
  std::string *page_href;
 int depth;
};

#endif

