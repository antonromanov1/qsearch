#pragma once

#include <string>
#include <vector>
#include <queue>
#include "thread_queue.h"

std::string gethtml(const std::string&);

std::string get_domain(const std::string&);
bool are_do_equal(std::vector<std::string>&);
bool is_in_cycle(std::vector<std::string>&);

std::string getUrl(std::string&, std::vector<std::string>&, Thread_queue<std::vector<std::string>>& );

int readbin(std::string&);

std::vector<std::string> parse(std::string&, std::string&);
void provide(Thread_queue<std::vector<std::string>>&);
void receive(Thread_queue<std::vector<std::string>>&, std::string&);
