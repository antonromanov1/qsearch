#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <string>
#include <iostream>
#include "search.h"

// Data storage format:
//
// (character '|' is used as a separator,
//  characters '{', '}' are used for defining repeated blocks)
//
// File keys.bin:
//========================================================================
// max_length_word - length of the longest word (sizeof(size_t) bytes) |
//
// {
// <word>\0\0...\0 (max_length_word bytes) |
// values_addr - address of respective urls in file values.bin
// (sizeof(unsigned long) bytes)
// ... 
// } (repeated)
//========================================================================
//
// File values.bin:
//========================================================================
// {
// quantity of urls (sizeof(size_t) bytes) |
// {
// length_of_url (sizeof(size_t) bytes) |
// url (length_of_urls bytes)
// ...
// } (repeated)
// ...
// } (repeated)
//========================================================================

std::list<std::string> search_word(std::string& word) {
    FILE *keys, *values;

    keys = fopen("keys.bin", "rb");
    values = fopen("values.bin", "rb");

    if (! keys) {
        perror("keys.bin");
        exit(-1);
    }
    if (! values) {
        perror("values.bin");
        exit(-1);
    }
    struct stat stat_buf;
    int fd = fileno(keys);
    fstat(fd, &stat_buf);

    size_t max_length_word;
    char *buffer;
    unsigned long values_addr;

    if (fread(&max_length_word, sizeof(size_t), 1, keys) != 1) {
        perror("");
        exit(-1);
    }

    buffer = new char [max_length_word + 1];
    buffer[max_length_word] = '\0';

    // Computing quantity of words
    unsigned long words_quantity = stat_buf.st_size - max_length_word;
    words_quantity = words_quantity / (max_length_word + sizeof(unsigned long)) + 1;

    // Binary search on keys.bin
    unsigned long left = 0;
    unsigned long right = words_quantity - 1;
    unsigned middle = 2;
    std::string current_word;
    int flag = 0;

    while (left <= right) {
        middle = (left + right) / 2;
        fseek(keys, sizeof(size_t) + (max_length_word + sizeof(unsigned long)) * middle,
                SEEK_SET);

        if (fread(buffer, sizeof(char), max_length_word, keys) != max_length_word) {
            perror("1");
            exit(-1);
        }
        if (fread(&values_addr, sizeof(unsigned long), 1, keys) != 1) {
            perror("2");
            exit(-1);
        }

        current_word = std::string(buffer);
        if (current_word < word)
            left = middle + 1;
        else if (current_word > word)
            right = middle - 1;
        else {
            flag = 1;
            goto LABEL;
        }
    }

LABEL:
    std::list<std::string> urls;
    char *url;

    if (flag == 1) {
        // values.bin file
        fseek(values, values_addr, SEEK_SET);
        size_t urls_quantity;
        if (fread(&urls_quantity, sizeof(size_t), 1, values) != 1) {
            perror("");
            exit(-1);
        }

        for (size_t i = 0; i < urls_quantity; ++i) {
            size_t url_length;
            if (fread(&url_length, sizeof(size_t), 1, values) != 1) {
                perror("");
                exit(-1);
            }

            url = new char [url_length + 1];
            if (fread(url, sizeof(char), url_length, values) != url_length) {
                perror("");
                exit(-1);
            }
            url[url_length] = '\0';
            // std::cout << url << std::endl;

            urls.push_back(url);
            delete url;
        }

        delete buffer;
        fclose(values);
        fclose(keys);
        return urls;
    }
    else {
        delete buffer;
        fclose(values);
        fclose(keys);
        std::cout << "Sorry, the system does not have word " << word <<
            std::endl;
        return urls;
    }

    exit(0);
}

std::list<std::string> search(size_t length, char **words) {
    if (length == 0)
        return std::list<std::string>();

    std::string word(words[0]);
    std::list<std::string> first_urls = search_word(word);
    std::list<std::string> urls;

    if (length == 1)
        return first_urls;

    for (size_t i = 1; i < length; ++i) {
        word = words[i];
        urls = search_word(word);
        if (urls.empty())
            return std::list<std::string>();

        for (auto iter1 = first_urls.begin(); iter1 != first_urls.end(); ) {

            std::string check_url = *iter1;
            int flag = 0;

            for (auto iter2 = urls.begin(); iter2 != urls.end(); ++iter2) {
                if (*iter2 == check_url)
                    flag = 1;
            }

            if (flag == 0)
                iter1 = first_urls.erase(iter1);
            else
                ++iter1;
        }
    }

    return first_urls;
}
