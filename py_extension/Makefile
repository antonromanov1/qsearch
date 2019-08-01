HEADERS= -I /usr/include/python3.7m -I ${PROJECTS_DIRECTORY}/qsearch/include
LIBRARIES= -L /usr/local/bin -lpython3
CPPFILES= ${PROJECTS_DIRECTORY}/qsearch/py_extension/searchresponse.cpp ${PROJECTS_DIRECTORY}/qsearch/py_extension/spammodule.c ${PROJECTS_DIRECTORY}/qsearch/common/search.cpp

CFLAGS= -g -Wall -o0
CC= g++

all:
	$(CC) $(CPPFILES) $(CFLAGS) $(HEADERS) -shared $(LIBRARIES) -o qsearch.so -fPIC
