#pragma once

#include <vector>
#include <string>
#include <list>

namespace qsearch {

class SearchResponse {
private:
    std::vector<std::string> words_vec;
    char **words;

    std::list<std::string> urls;

public:
    SearchResponse(const char *);

    std::list<std::string> get_urls();
    size_t get_urls_size();

    ~SearchResponse();
};

}
