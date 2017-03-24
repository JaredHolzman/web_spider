#ifndef _PAGE_H
#define _PAGE_H

#include <string>

class Page {
public:
  Page(std::string *page_href, std::string *parent, int depth);
  std::string * get_href();
  std::string * get_parent();
  int get_depth();

private:
  std::string *page_href;
  std::string *parent;
 int depth;
};

#endif

