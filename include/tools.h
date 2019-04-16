#pragma once

#include <string>
#include <vector>
#include <queue>
#include "thread_queue.h"

struct Page {
    std::string url;
    std::string title;
    std::string text;
};

std::string gethtml(const std::string&);

std::string get_domain(const std::string&);
bool are_do_equal(std::vector<std::string>&);
bool is_in_cycle(std::vector<std::string>&);

int readbin(std::string&);

Page parse(std::string&, std::string&);
void provide(Thread_queue<Page>&);
size_t fwrite_str(std::string&, FILE*);
void receive(Thread_queue<Page>&, std::string&);
