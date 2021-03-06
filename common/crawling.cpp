#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstddef>
#include <ctime>
#include <sys/stat.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <iterator>
#include <stdexcept>
#include <mutex>
#include <utility>
#include "crawling.h"
#include "Document.h"
#include "Node.h"

#define THREAD_OUTPUT std::cout << "Thread " << thread_index << " - saved " << url << std::endl;

namespace qsearch {

std::mutex mutex;

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
    size_t new_len = s->len + size * nmemb;
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
    long expected_code = 200.0;
    std::string message("curl_error");
    CURL *curl;
    struct struct_string s;

    curl = curl_easy_init();

    if(curl) {
        init_struct_string(&s);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str() );
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);
        CURLcode res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            long response_code;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
            if (response_code != expected_code)
                return message;
        }
        else if (res != CURLE_OK)
            return message;

        curl_easy_cleanup(curl); /* always cleanup */
    }
    else
        return message;

    std::string f(s.ptr);
    free(s.ptr);

    return f;
}

std::string get_domain(const std::string& url)
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

bool are_do_equal(std::vector<std::string>& v)
{
    if (v.size() <= 1)
        return true;
    for (auto iter = v.begin() + 1; iter != v.end(); iter++)
        if (get_domain(*iter) != get_domain(*(iter - 1) ) )
            return false;
    return true;
}

bool is_in_cycle(std::vector<std::string>& urls)
{
    std::string domen;

    if (urls.size() == 0)
        return false;
    else {
        domen = get_domain(urls[0]);

        for (auto iter = urls.begin() + 1; iter != urls.end(); ++iter)
            if (get_domain(*iter) != domen)
                return false;
    }

    return true;
}

Page parse(std::string& url, std::string& html) {
    CDocument doc;
    doc.parse(html.c_str());

    CSelection c = doc.find("title");
    std::string title;
    try {
        title = c.nodeAt(0).text();
    }
    catch (const std::out_of_range& ex) {
        title = "<<<Has not title>>>";
    }
    std::string page_text(doc.page_text());

    Page page  = { .url = url, .title = title, .text = page_text};

    return page;
}

static size_t fwrite_str(const std::string& str_to_disk, FILE* fp) {
    size_t result = 0;
    size_t length;

    length = (size_t) str_to_disk.size();
    result += fwrite(&length, sizeof(size_t), 1, fp); // fwrite should return 1

    result += fwrite(str_to_disk.c_str(), sizeof(char), length, fp); // fwrite should return length

    return result;
}

static void write_page(const Page& page, std::string& file_name) {
    FILE * fp = fopen(file_name.c_str(), "ab");
    if (! fp) {
        std::cout << strerror(errno) << std::endl;
        exit(-1);
    }

    mutex.lock();

    fwrite_str(page.url, fp);
    fwrite_str(page.title, fp);
    fwrite_str(page.text, fp);

    mutex.unlock();

    fclose(fp);
}

void walk_internet(std::string file_name, size_t thread_index) {
    std::vector<std::string> base_links;
    base_links.push_back("https://www.theguardian.com/international");
    base_links.push_back("https://www.nbcnews.com/");
    base_links.push_back("https://www.allrecipes.com/");
    base_links.push_back("https://en.wikipedia.org/wiki/Main_Page");

    std::vector<std::string> vector_of_links;

    srand(time(NULL));
    std::string url = base_links[rand() % base_links.size()];
    std::string home_url = url;

    std::string html(gethtml(url));
    CDocument doc_1;
    doc_1.parse(html.c_str());
    std::vector<std::string> links(doc_1.get_links());
    Page page = parse(url, html);
    write_page(page, file_name);
    THREAD_OUTPUT

    for (;;) {
        vector_of_links.push_back(std::move(url));

        url = links[rand() % links.size()];

        if (url.substr(0, 4) != "http")
            url = home_url + url;

        html = gethtml(url);
        if (html != "curl_error") {
            page = parse(url, html);
            write_page(page, file_name);
            THREAD_OUTPUT
        }
        else {
            std::cout << "----------------Exception------------" << std::endl;
            url = base_links[rand() % base_links.size()];
            html = gethtml(url);
            page = parse(url, html);
            write_page(page, file_name);
            THREAD_OUTPUT
        }

        CDocument doc;
        doc.parse(html.c_str());
        links = doc.get_links();

        if (links.size() == 0) {
            url = base_links[rand() % base_links.size()];
            html = gethtml(url);
            CDocument doc2;
            doc2.parse(html.c_str());
            links = doc2.get_links();
            page = parse(url, html);
            write_page(page, file_name);
            THREAD_OUTPUT
        }
        else
            ;

        // If the crawler got in a cycle
        if (vector_of_links.size() > 5) {
            std::vector<std::string> last_links;

            for (int i = 0; i < 5; ++i)
                last_links.push_back(vector_of_links[vector_of_links.size() - (-i + 5)]);

            if (is_in_cycle(last_links)) {
                std::cout << "--------------Got in a cycle----------------" << std::endl;
                url = base_links[rand() % base_links.size()];
                html = gethtml(url);
                page = parse(url, html);
                write_page(page, file_name);
                THREAD_OUTPUT
            }

        }

        home_url = url;
    }
}

int read_primary(std::string& file_name)
{
    FILE *fp = fopen(file_name.c_str(), "rb");
    if (!fp){
        perror("");
        return -1;
    }

    struct stat stat_buf;
    int fd = fileno(fp);
    fstat(fd, &stat_buf);
    unsigned long long i = 0;
    Page page;
    unsigned char reminder;

    size_t len;
    char*  buffer;
    char* str;

    for(unsigned long long j = 0; i < 0.95 * stat_buf.st_size; ++j) {
        if (fread(&len, sizeof(size_t), 1, fp) != 1) {
            perror("");
            fclose(fp);
            return -1;
        }
        i += sizeof(size_t);

        buffer = new char [len];
        str = new char [len + 1];

        if (fread(buffer, sizeof(char), len, fp) != len) {
            perror("");
            fclose(fp);
            return -1;
        }
        i += sizeof(char) * len;

        for (size_t i = 0; i < len; ++i)
            str[i] = buffer[i];
        str[len] = '\0';

        reminder = j % 3;
        switch (reminder) {
            case 0: page.url = str;
                    std::cout << str << std::endl;
                    break;
            case 1: page.title = str;
                    break;
            case 2: page.text = str;
                    break;
        }

        delete str;
        delete buffer;
    }
    std::cout << "It has read " << i / 1024 << " kb" << std::endl;

    fclose(fp);
    return 0;
}

} // namespace
