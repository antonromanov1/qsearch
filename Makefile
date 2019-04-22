P=main
HEADERS= -I ${PROJECTS_DIRECTORY}/qsearch/include -I ${PROJECTS_DIRECTORY}/qsearch/gumbo-wrapper
MAIN= src/main.cpp
CPPFILES= src/crawling.cpp src/handling_primary.cpp gumbo-wrapper/*.cpp

CFLAGS= -g -Wall -o0
LDLIBS= -lcurl -pthread `pkg-config --cflags --libs gumbo`
CC= g++

$(P):
	$(CC) $(CFLAGS) $(HEADERS) $(CPPFILES) $(MAIN) -o $@ $(LDLIBS)

clean:
	rm $(P)
