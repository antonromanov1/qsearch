#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <thread>
#include <functional>
#include "tools.h"
#include "thread_queue.h"

int main(int argc, char *argv[])
{
    std::string file_name = "data.bin";

    if (argc == 1)
        std::cout << "If you want to crawl the Internet add argument \"crawl\"" <<
            std::endl << "if you want to read crawled data add argument \"read\"" << std::endl;

    else if (argc == 2){
        if (strcmp(argv[1], "crawl") == 0){
            Thread_queue<std::vector<std::string>> queue;

            const unsigned char NUMBER = 1;
            std::thread* threads[NUMBER];

            for (unsigned char i = 0; i < NUMBER; ++i) {
                threads[i] = new std::thread(provider, std::ref(queue));
            }
            std::thread thr(receiver, std::ref(queue), std::ref(file_name));

            thr.join();
            for (unsigned char i = 0; i < NUMBER; ++i) {
                threads[i]->join();
                delete threads[i];
            }
        }

        else if (strcmp(argv[1], "read") == 0){
            unsigned int number = 10;
            std::cout << "Output of the first " << number << " crawled pages" << std::endl;
            readbin(file_name);
        }
    }
    else
        return -1;

    return 0;
}
