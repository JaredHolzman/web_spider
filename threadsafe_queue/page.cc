#include "page.h"

Page::Page(std::string *page_href, std::string *parent, int depth)
    : page_href(page_href), parent(parent), depth(depth) {}
std::string *Page::get_href() { return page_href; }
std::string *Page::get_parent() { return parent; }
int Page::get_depth() { return depth; }
