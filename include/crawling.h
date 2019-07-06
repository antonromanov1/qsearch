#pragma once

#include <string>
#include <vector>
#include <queue>
#include "thread_queue.h"
#include "page.h"

std::string gethtml(const std::string&);

std::string get_domain(const std::string&);
bool are_do_equal(std::vector<std::string>&);
bool is_in_cycle(std::vector<std::string>&);

Page parse(std::string&, std::string&);
void provide(Thread_queue<Page>&);
void receive(Thread_queue<Page>&, std::string&);
int read_primary(std::string&);
