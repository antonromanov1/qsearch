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
#include "crawling.h"
#include "thread_queue.h"
#include "handling_primary.h"
#include "search.h"

int main(int argc, char *argv[])
{
    std::string file_name_primary_data = "data.bin";

    if (argc == 1)
        std::cout << "If you want to crawl the Internet add argument \"crawl\"" <<
        std::endl << "               read crawled data add argument \"read_primary\"" <<
        std::endl << "               handle crawled data add argument \"handling\"" <<
        std::endl << "               search your word add argument \"search\" and further <word>" <<
        std::endl;

    else if (argc == 2) {
        if (strcmp(argv[1], "crawl") == 0) {
            Thread_queue<Page> queue;

            const unsigned char NUMBER = 1;
            std::thread* threads[NUMBER];

            for (unsigned char i = 0; i < NUMBER; ++i) {
                threads[i] = new std::thread(provide, std::ref(queue));
            }
            std::thread thr(receive, std::ref(queue), std::ref(file_name_primary_data));

            thr.join();
            for (unsigned char i = 0; i < NUMBER; ++i) {
                threads[i]->join();
                delete threads[i];
            }
        }

        else if (strcmp(argv[1], "read_primary") == 0) {
            std::cout << "Content of data.bin:" << std::endl;
            read_primary(file_name_primary_data);
        }

        else if (strcmp(argv[1], "handling") == 0) {
            FILE* fp = fopen("data.bin", "rb");
            if (!fp) {
                perror("data.bin");
                return -1;
            }
            time_t start = time(NULL);
            handling(fp);
            time_t end = time(NULL);
            std::cout << end - start << " seconds" << std::endl;
            fclose(fp);
        }

    }

    else if (argc == 3) {
        if (strcmp(argv[1], "search") == 0) {
            std::string word(argv[2]);
            search(word);
        }

        else
            return -1;
    }

    else
        return -1;

    return 0;
}
