#include <vector>
#include <string>
#include <regex>

#include "document.h"

Document::Document() { }

Document::Document(const std::string& html) {
    std::string patt_title = "<title>(.*?)</title>";
    std::string patt_links = "<\\s*A\\s+(?:[^>]*?\\s+)?href\\s*=\\s*\"([^\"]*)\"";

    std::regex re(patt_title, std::regex_constants::icase);
    std::regex re2(patt_links, std::regex_constants::icase);

    std::vector<std::string> titles(std::sregex_token_iterator(html.begin(), html.end(), re, 1),
                               std::sregex_token_iterator());

    std::vector<std::string> a_links(std::sregex_token_iterator(html.begin(), html.end(), re2, 1),
                               std::sregex_token_iterator());

    if (titles.size() == 0) {
        std::string str1("");
        doc_title = str1;
    }
    else {
        std::string str2(titles[0]);
        doc_title = str2;
    }

    if (a_links.size() == 0) {
        std::vector<std::string> vec1;
        doc_links = vec1;
    }
    else {
        std::vector<std::string> vec2(a_links);
        doc_links = vec2;
    }
}

Document::~Document() {
}
