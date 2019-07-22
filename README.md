# QSearch

QSearch is a search engine which consists of a multithreaded web crawler,
a handler of the crawled data and a tool for searching.

## Gumbo (a pure-C HTML5 parser) and gumbo-query

I have edited a gumbo C++ wrapper gumbo-query (https://github.com/lazytiger/gumbo-query).

Changes I made are below:

   1. Added function search_for_links (copied and modified from gumbo-parser/examples
   https://github.com/google/gumbo-parser)
   2. Added member function CDocument::get_links
   3. Edited destructor of CObject and member function CObject::release
   4. Added member function CSelection::results
   5. Added member function CDocument::page_text

# How to use it
You can build QSearch with:

```bash
make
```

run the crawler with:

```bash
./main crawl
```

prepare the system for searching with:

```bash
./main handling_primary
```

and at last search your query with:

```bash
./main search <query>
```
