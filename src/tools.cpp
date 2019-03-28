#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <ctime>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <regex>
#include <iterator>
#include "tools.h"
#include "document.h"
#include "thread_queue.h"

struct struct_string {
    char *ptr;
    size_t len;
};

void init_struct_string(struct_string *s) {
    s->len = 0;
    s->ptr = (char*)malloc(s->len + 1);

    if (s->ptr == NULL) {
        std::cerr << "malloc() failed\n" << std::endl;
        exit(EXIT_FAILURE);
    }

    s->ptr[0] = '\0';
}

size_t writefunc(void *ptr, size_t size, size_t nmemb, struct struct_string *s)
{
    size_t new_len = s->len + size*nmemb;
    void* ptr2 = realloc(s->ptr, new_len + 1);

    if (ptr2 == NULL) {
        std::cerr << "realloc() failed\n" << std::endl;
        free(s->ptr);
        exit(EXIT_FAILURE);
    }
    s->ptr = (char*)ptr2;

    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;

    return size * nmemb;
}

std::string gethtml(const std::string& url)
{
    CURL *curl;
    //CURLcode res;
    struct struct_string s;
    std::string f;

    curl = curl_easy_init();

    if(curl) {
        init_struct_string(&s);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res) );

        /* always cleanup */
        curl_easy_cleanup(curl);
    }

    f = std::string(s.ptr);
    free(s.ptr);

    return f;
}

std::string getDomain(const std::string& url)
{
    std::string prot = url.substr(0, 5);
    const char *urlStr = url.c_str();
    char domain[1024];

    if (prot == "http:") {
        if (sscanf(urlStr, "http://%[^/]", domain))
            ;
        return std::string(domain);
    }
    else if (prot == "https") {
        if (sscanf(urlStr, "https://%[^/]", domain))
            ;
        return std::string(domain);
    }
    else
        throw std::bad_function_call();
}

bool isDoEqual(std::vector<std::string>& v)
{
    if (v.size() <= 1)
        return true;
    for (auto iter = v.begin() + 1; iter != v.end(); iter++)
        if (getDomain(*iter) != getDomain(*(iter - 1) ) )
            return false;
    return true;
}

bool isInCycle(std::vector<std::string>& urls)
{
    const int num = 20;
    auto sz = urls.size();

    if (sz < num)
        return false;
    else if (sz > num) {
        std::vector<std::string> lastUrls(urls.end() - num, urls.end() );
        if (isDoEqual(lastUrls))
            return true;
        else
            return false;
    }
    else
        return false;
}

const unsigned short enoughNumber = 50;

std::string getUrl(std::string& url, std::vector<std::string>& base_links,
                   Thread_queue<std::vector<std::string>>& queue)
{
    /*
     * gets html, parses it, if links > enoughNumber pushes at the end of base_links current URL
     * writes (id, length of title, title, length of links, length of link0, link0, ...) at the end of the file
     */
    if ( (url.size() < 10) ||  (url[0] != 'h') || (url[1] != 't') || (url[2] != 't') ) {
        std::cout << "Not a URL" << std::endl;
        srand(time(0));
        url = base_links[rand() % base_links.size()];
    }

    std::string html = gethtml(url);
    Document doc(html);
    std::string title(doc.title());
    std::vector<std::string> links = doc.links();

    srand(time(0));
    if (title.size() == 0)
        return base_links[rand() % base_links.size()];

    if (links.size() > enoughNumber)
        base_links.push_back(url);

    srand(time(0));
    std::string res;
    std::string rstr;
    if (links.size() == 0){
        std::cout << "No links found, URL:" << url << std::endl;
        return base_links[rand() % base_links.size()];
    }
    else {
        rstr = links[rand() % links.size()];
        if (rstr.substr(0, 4) != "http")
            res = url + rstr;
        else
            res = url;
    }

    std::vector<std::string> vec;
    vec.push_back(url);
    vec.push_back(title);

    queue.push(vec);

    return res;
}

int readbin(unsigned int number)
{
    FILE *ptr_f = fopen("data.bin", "rb");
    if (!ptr_f){
        std::cout << strerror(errno) << std::endl;
        return -1;
    }

    size_t len;
    char*  buffer;
    
    for (unsigned int i = 0; i != number * 2; i++){

        if (fread(&len, sizeof(size_t), 1, ptr_f) != 1)
            std::cout << strerror(errno) << std::endl;

        buffer = new char [len];
        if (fread(buffer, sizeof(char), len, ptr_f) != len)
            std::cout << strerror(errno) << std::endl;

        std::cout << buffer << std::endl;
        delete buffer;
    }

    fclose(ptr_f);
    return 0;
}

void provider(Thread_queue<std::vector<std::string>>& queue) {
    std::vector<std::string> base_urls;
    base_urls.push_back("https://en.wikipedia.org/wiki/Difference_engine");
    base_urls.push_back("https://www.gnu.org/home.en.html");
    base_urls.push_back("https://www.nytimes.com/");
    base_urls.push_back("https://www.wsj.com/europe");

    std::vector<std::string> urls_all;

    srand(std::time(0));
    std::string url = base_urls[rand() % base_urls.size()];
    unsigned long id = 1;
    for(;;){
        // std::cout << "ID: " << id << "; URL:" << url << std::endl;
        urls_all.push_back(url);
        if ((url = getUrl(url, base_urls, queue) ) == "Error")
            throw std::runtime_error("error in thread_func() call");

        id++;
        if (isInCycle(urls_all)) {
            // std::cout << "In cycle" << std::endl;
            srand(time(0));
            url = base_urls[rand() % base_urls.size()];
        }
    }
}

void receiver(Thread_queue<std::vector<std::string>>& queue, std::string& file_name) {
    FILE * ptr_f = fopen(file_name.c_str(), "wb");
    if (!ptr_f) {
        std::cout << strerror(errno) << std::endl;
    }

    for (;;) {
        if (queue.not_empty()) {
            std::vector<std::string> vec(queue.front());
            queue.pop();

            std::string url(vec[0]);
            std::string title(vec[1]);

            // writing url
            size_t length = (size_t)(url.size());
            size_t* buffer = &length;
            if (fwrite(buffer, sizeof(size_t), 1, ptr_f) != 1) {
                std::cout << strerror(errno) << std::endl;
            }

            char* buffer2 = new char [length + 1];
            strcpy(buffer2, url.c_str());
            if (fwrite(buffer2, sizeof(char), length, ptr_f) != length) {
                std::cout << strerror(errno) << std::endl;
            }
            delete buffer2;

            // writing title
            length = (size_t)(title.size());
            buffer = &length;
            if (fwrite(buffer, sizeof(size_t), 1, ptr_f) != 1) {
                std::cout << strerror(errno) << std::endl;
            }

            buffer2 = new char [length + 1];
            strcpy(buffer2, title.c_str());
            if (fwrite(buffer2, sizeof(char), length, ptr_f) != length) {
                std::cout << strerror(errno) << std::endl;
            }
            delete buffer2;
        }
    }

    fclose(ptr_f);
}