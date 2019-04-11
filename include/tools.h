#pragma once

#include <string>
#include <vector>
#include <queue>
#include "thread_queue.h"

std::string gethtml(const std::string&);

std::string getDomain(const std::string&);
bool isDoEqual(std::vector<std::string>&);
bool isInCycle(std::vector<std::string>&);

std::string getUrl(std::string&, std::vector<std::string>&, Thread_queue<std::vector<std::string>>& );

int readbin(std::string&);

void provider(Thread_queue<std::vector<std::string>>&);
void receiver(Thread_queue<std::vector<std::string>>&, std::string&);
