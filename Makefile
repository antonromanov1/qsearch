P=main
HEADERS= -I ${PROJECTS_DIRECTORY}/qsearch/include -I ${PROJECTS_DIRECTORY}/qsearch/gumbo-wrapper
MAIN= common/main.cpp
CPPFILES= common/crawling.cpp common/handling_primary.cpp common/search.cpp gumbo-wrapper/*.cpp

CFLAGS= -g -Wall -o0
LDLIBS= -lcurl -pthread `pkg-config --cflags --libs gumbo`
CC= g++

$(P):
	$(CC) $(CFLAGS) $(HEADERS) $(CPPFILES) $(MAIN) -o $@ $(LDLIBS)

clean:
	rm $(P)

web:
	$(CC) $(CFLAGS) $(HEADERS) common/search.cpp ui/response.cpp -o /var/www/cgi-bin/response.cgi -lcgicc
