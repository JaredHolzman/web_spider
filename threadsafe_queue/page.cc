#include "page.h"

Page::Page(std::string *page_href, int depth) : page_href(page_href), depth(depth) {}
std::string *Page::get_href() { return page_href; }
int Page::get_depth() { return depth; }
