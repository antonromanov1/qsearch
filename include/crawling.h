#pragma once

#include <string>
#include <vector>
#include <queue>

namespace qsearch {

struct Page {
    std::string url;
    std::string title;
    std::string text;
};

std::string gethtml(const std::string&);

std::string get_domain(const std::string&);
bool are_do_equal(std::vector<std::string>&);
bool is_in_cycle(std::vector<std::string>&);

Page parse(std::string&, std::string&);
void walk_internet(std::string, size_t);
int read_primary(std::string&);

}
