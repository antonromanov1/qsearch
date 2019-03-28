P=main
HEADERS= -I ${PROJECTS_DIRECTORY}/qsearch/include
MAIN= src/main.cpp
CPPFILES= src/tools.cpp src/document.cpp

CFLAGS= -g -Wall -o3
LDLIBS= -lcurl -pthread
CC= g++

$(P):
	$(CC) $(CFLAGS) $(HEADERS) $(CPPFILES) $(MAIN) -o $@ $(LDLIBS)

rm:
	rm $(P)
