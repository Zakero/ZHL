/*
Copy and Paste the following into your terminal of choice.
Must be in the "test/" directory.

- Compile and Run
g++ -std=c++20 -Wall -Werror -o MemoryPool *.cpp && ./MemoryPool
clang++ -std=c++20 -Wall Werror -o MemoryPool *.cpp && ./MemoryPool

- Code Coverage
g++ -fprofile-arcs -ftest-coverage -fPIC -O0 -std=c++2a -Wall -Werror -o MemoryPool *.cpp && ./MemoryPool
gcovr -j 8 -r .. --html --html-details -o MemoryPool-Code-Coverage.html
*/

#define ZAKERO_MEMORYPOOL_IMPLEMENTATION
//#define ZAKERO_MEMORYPOOL_ENABLE_DEBUG
#include "../../include/Zakero_MemoryPool.h"

#define CATCH_CONFIG_MAIN
#include "../catch.hpp"

