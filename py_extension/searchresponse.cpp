#include <vector>
#include <list>
#include <string>
#include <iostream>
#include "search.h"
#include "searchresponse.h"

namespace qsearch {

static std::vector<std::string> split(const char *str) {
    std::vector<std::string> ret_vec;
    std::string s(str);
    std::string delimiter = " ";

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        ret_vec.push_back(token);
        s.erase(0, pos + delimiter.length());
    }

    if (! ret_vec.empty())
        return ret_vec;
    else {
        ret_vec.push_back(str);
        return ret_vec;
    }
}

SearchResponse::SearchResponse(const char *request) {
    words_vec = split(request);
    size_t len = words_vec.size();

    words = new char* [len];
    
    for (size_t i = 0; i < len; ++i)
        words[i] = (char*) words_vec[i].c_str();

    urls = search(len, words);
}

std::list<std::string> SearchResponse::get_urls() {
    return urls;
}

size_t SearchResponse::get_urls_size() {
    return urls.size();
}

SearchResponse::~SearchResponse() {
    delete[] words;
}

} // namespace
