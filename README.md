# qsearch

This project represents a basic multithreaded web crawler which is written in C++.

I have edited the C++ gumbo wrapper (https://github.com/lazytiger/gumbo-query).

Changes I made are below:

   1. Added function search_for_links (copied and modified from gumbo-parser/examples
   https://github.com/google/gumbo-parser)
   2. Added member function CDocument::get_links
   3. Edited destructor of CObject and member function CObject::release
   4. Added member function CSelection::results
   5. Added member function CDocument::page_text
