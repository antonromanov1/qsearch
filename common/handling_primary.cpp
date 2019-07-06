#include <sys/stat.h>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <ctime>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <utility>
#include "page.h"
#include "crawling.h"

static void remove_duplicates(std::vector<std::string>& arg_vec){
    std::sort(arg_vec.begin(), arg_vec.end());
    arg_vec.erase(std::unique(arg_vec.begin(), arg_vec.end()), arg_vec.end());
}

static std::vector<std::string> split(const std::string& input_str, const char delim) {
    std::vector<std::string> strings;
    std::istringstream f(input_str);
    std::string s;

    while (getline(f, s, delim))
        strings.push_back(s);

    return strings;
}

static void clean_word(std::string& x) {
    std::string::size_type len = x.size();
    
    // deleting apostrophe which is not in ascii and letter 's' from the end of the word
    const char apost[] = "’s";
    if ((len > strlen(apost)))
        if (x.substr(len - strlen(apost), len) == apost) {
            x.erase(len - strlen(apost), len);
            len = x.size();
        }

    std::string sub;
    if (len > 2) {
        sub = x.substr(len - 2, len);
        if ((sub == "\'s") || (sub == "\',")) {    // cases such as "Ecuador\'s" or "spying\',"
            x.erase(len - 2, len);
            len = x.size();
        }
    }

    // deleting last character if it is a punctuation sign
    char temp = *(x.end() - 1);
    if (ispunct(temp))
        x.erase(len - 1);

    // the same with first character
    temp = *x.begin();
    if (ispunct(temp)) {
        x.erase(x.begin());
        temp = *x.begin();
    }

    // lowering each symbol if needed
    for (auto iter = x.begin(); iter != x.end(); ++iter)
        if (isupper(*iter))
            *iter = tolower(*iter);
}

static std::vector<std::string> get_words(std::vector<std::string>& input_vector) {
    std::vector<std::string> result;
    std::string::size_type i;

    for (std::string x : input_vector) {
        i = 0;
        clean_word(x);

        while (isalpha(x[i]))
            ++i;

        if ((x.size() == i) && (i != 0))
            result.push_back(x);
    }

    return result;
}

static int write_words(const std::map<std::string, std::vector<std::string>>& words, size_t max_length_word) {
    FILE* keys = fopen("keys.bin", "wb");
    if (!keys) {
        perror("");
        return -1;
    }
    FILE* values = fopen("values.bin", "wb");
    if (!values) {
        perror("");
        return -1;
    }

    fwrite(&max_length_word, sizeof(size_t), 1, keys);

    size_t buffer_size = sizeof(char) * max_length_word;
    char* buffer = (char*)malloc(buffer_size); // char buffer[max_length_word];
    unsigned long values_addr = 0;
    size_t urls_quantity;
    for (auto x : words) {
        memcpy(buffer, x.first.c_str(), x.first.size());
        for (char* address = buffer + x.first.size(); address != buffer + buffer_size; ++address)
            *address = '\0';

        fwrite(buffer, sizeof(char), buffer_size, keys);

        // writing to file "keys.bin" address of urls from values.bin
        fwrite(&values_addr, sizeof(unsigned long), 1, keys);

        urls_quantity = x.second.size();
        fwrite(&urls_quantity, sizeof(size_t), 1, values);

        values_addr += (unsigned long)sizeof(size_t);

        for (auto y : x.second) {
            size_t length_of_url = y.size();
            fwrite(&length_of_url, sizeof(size_t), 1, values);

            fwrite(y.c_str(), sizeof(char), y.size(), values);
            values_addr += sizeof(size_t) + y.size();
        }
    }

    free(buffer);
    fclose(keys);
    fclose(values);
    return 0;
}

int handling(FILE* fp)
{
    struct stat stat_buf;
    int fd = fileno(fp);
    fstat(fd, &stat_buf);

    std::vector<std::string> for_split;
    std::vector<std::string> page_words;
    std::map<std::string, std::vector<std::string>> words;
    unsigned long long i = 0;
    Page page;
    std::vector<Page> pages;
    unsigned char remainder;

    size_t len;
    char*  buffer;
    char* str;

    time_t start = time(NULL);
    for(unsigned long long j = 0; i < 0.95 * stat_buf.st_size; ++j) {
        if (fread(&len, sizeof(size_t), 1, fp) != 1) {
            perror("");
            return -1;
        }
        i += sizeof(size_t);

        buffer = new char [len];
        str = new char [len + 1];

        if (fread(buffer, sizeof(char), len, fp) != len) {
            perror("");
            return -1;
        }
        i += sizeof(char) * len;

        for (size_t i = 0; i < len; ++i)
            str[i] = buffer[i];
        str[len] = '\0';

        remainder = j % 3;
        switch (remainder) {
            case 0: page.url = str;
            case 1: page.title = str;
            case 2: page.text = str;
        }

        if (remainder == 2)
            pages.push_back(page);

        delete str;
        delete buffer;
    }
    time_t finish = time(NULL);
    std::cout << "File has been read, " << (finish - start) / 60.0 << " minutes" << std::endl;

    start = time(NULL);
    std::vector<std::string> temp;
    for (auto elem : pages){
        for_split = split(elem.text, ' ');
        page_words = get_words(for_split);

        for (std::string x : page_words)
            if (words.find(x) == words.end()) {
                temp.clear();
                temp.push_back(elem.url);
                words[x] = temp;
            }
            else
                words[x].push_back(elem.url);
    }
    finish = time(NULL);
    std::cout << "map \"words\" is constructed in " << (finish - start) / 60.0 << " min" << std::endl;

    size_t max_length_word = 0;
    for (auto& x : words) {
        if (x.first.size() > max_length_word)
            max_length_word = x.first.size();

        remove_duplicates(x.second);
    }

    /*
    for (auto it = words.begin(); it != words.end(); ++it) {
        std::cout << "=======================================================================" << std::endl;
        std::cout << "                " << it->first << std::endl;
        std::cout << "=======================================================================" << std::endl;

        for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
            std::cout << *it2 << std::endl;
    }
    */
    write_words(words, max_length_word);
    std::cout << "The system has " << words.size() << " words" << std::endl;

    return 0;
}
