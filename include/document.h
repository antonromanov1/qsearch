#pragma once

#include <vector>
#include <string>

class Document {
private:
    std::string doc_title;
    std::vector<std::string> doc_links;

public:
    Document();
    Document(const std::string &);
    ~Document();

    std::string title(void) const { return doc_title; }

    std::vector<std::string> links(void) const { return doc_links; }
};
