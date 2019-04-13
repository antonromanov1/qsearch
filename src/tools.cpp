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
#include <functional>
#include <regex>
#include <iterator>
#include <stdexcept>
#include "tools.h"
#include "Document.h"
#include "Node.h"
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
    long expected_code = 200;
    std::string message("curl_error");
    CURL *curl;
    //CURLcode res;
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

std::string getUrl(std::string& url, std::vector<std::string>& base_links,
                   Thread_queue<std::vector<std::string>>& queue)
{
    /*
     * gets html, parses it, if links > enoughNumber pushes at the end of base_links current URL
     * writes data to queue
     */

    const unsigned short enoughNumber = 50;
    if ( (url.size() < 10) || (url.substr(0, 4) != "http") ) {
        std::cout << "Not a URL" << std::endl;
        srand(time(0));
        url = base_links[rand() % base_links.size()];
    }

    std::string html = gethtml(url);
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

    std::vector<std::string> links = doc.get_links();

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
    vec.push_back(page_text);

    queue.push(vec);

    return res;
}

int readbin(std::string& file_name)
{
    FILE *fp = fopen(file_name.c_str(), "rb");
    if (!fp){
        std::cout << strerror(errno) << std::endl;
        return -1;
    }

    struct stat stat_buf;
    int fd = fileno(fp);
    fstat(fd, &stat_buf);
    unsigned long int length = stat_buf.st_size;
    unsigned long int i = 0;

    size_t len;
    char*  buffer;
    
    while (i < length + 1) {

        if (fread(&len, sizeof(size_t), 1, fp) != 1) {
            std::cout << strerror(errno) << std::endl;
        }
        i += sizeof(size_t);

        buffer = new char [len];

        if (fread(buffer, sizeof(char), len, fp) != len) {
            std::cout << strerror(errno) << std::endl;
        }
        i += sizeof(char) * len;

        std::cout << buffer << std::endl;

        delete buffer;
    }

    fclose(fp);
    return 0;
}

std::vector<std::string> parse(std::string& url, std::string& html) {
    CDocument doc;
    doc.parse(html.c_str());
    std::vector<std::string> links = doc.get_links();

    CSelection c = doc.find("title");
    std::string title;
    try {
        title = c.nodeAt(0).text();
    }
    catch (const std::out_of_range& ex) {
        title = "<<<Has not title>>>";
    }
    std::string page_text(doc.page_text());

    std::vector<std::string> vec;
    vec.push_back(url);
    vec.push_back(title);
    vec.push_back(page_text);

    return vec;
}

void provide(Thread_queue<std::vector<std::string>>& queue) {
    std::vector<std::string> base_links;
    base_links.push_back("https://www.theguardian.com/international");
    base_links.push_back("https://www.nbcnews.com/");
    base_links.push_back("https://www.allrecipes.com/");
    base_links.push_back("https://en.wikipedia.org/wiki/Difference_engine");

    std::vector<std::string> vector_of_links;

    srand(time(NULL));
    std::string url = base_links[rand() % base_links.size()];
    std::string home_url = url;
    std::cout << "Fetching " << url << "..." << std::endl;

    std::string html(gethtml(url));
    CDocument doc_1;
    doc_1.parse(html.c_str());
    std::vector<std::string> links(doc_1.get_links());
    std::vector<std::string> vec(parse(url, html));
    queue.push(vec);

    for (;;) {
        std::cout << std::endl;
        vector_of_links.push_back(url);

        url = links[rand() % links.size()];

        if (url.substr(0, 4) != "http")
            url = home_url + url;

        std::cout << "Fetching " << url << std::endl;

        html = gethtml(url);
        if (html != "curl_error") {
            vec = parse(url, html);
        }
        else {
            std::cout << "----------------Exception------------" << std::endl;
            url = base_links[rand() % base_links.size()];
            html = gethtml(url);
            vec = parse(url, html);
            queue.push(vec);
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
            vec = parse(url, html);
            queue.push(vec);
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
                vec = parse(url, html);
                queue.push(vec);
            }

        }

        home_url = url;
    }
}

void receive(Thread_queue<std::vector<std::string>>& queue, std::string& file_name) {
    FILE * ptr_f = fopen(file_name.c_str(), "wb");
    if (!ptr_f) {
        std::cout << strerror(errno) << std::endl;
    }

    for (;;) {
        if (queue.not_empty()) {
            std::vector<std::string> vec(queue.front());
            queue.pop();

            size_t length;
            size_t* buffer;
            char* buffer2;

            for (std::string x : vec) { // vec has exactly 3 elements
                length = (size_t) x.size();
                buffer = &length;
                if (fwrite(buffer, sizeof(size_t), 1, ptr_f) != 1) {
                    std::cout << strerror(errno) << std::endl;
                }

                buffer2 = new char [length + 1];
                strcpy(buffer2, x.c_str());
                if (fwrite(buffer2, sizeof(char), length, ptr_f) != length) {
                    std::cout << strerror(errno) << std::endl;
                }
                delete buffer2;
            }

        }
    }

    fclose(ptr_f);
}
