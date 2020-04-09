cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
# N.B. this probably does not make much sense to try on a non Linux platform
project(Isoalloc LANGUAGES CXX C)

#SECURITY_FLAGS = -DSANITIZE_CHUNKS=0
#THREAD_SUPPORT = -DTHREAD_SUPPORT=1 -pthread
#PRE_POPULATE_PAGES = -DPRE_POPULATE_PAGES=1
#CFLAGS = $(COMMON_CFLAGS) $(SECURITY_FLAGS) -fvisibility=hidden -std=c11
#CXXFLAGS = $(COMMON_CFLAGS) -DCPP_SUPPORT -std=c++11
#
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSANITIZE_CHUNKS=0 -DTHREAD_SUPPORT=1 -DPRE_POPULATE_PAGES=1 -pthread -fvisibility=hidden -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSANITIZE_CHUNKS=0 -DTHREAD_SUPPORT=1 -DPRE_POPULATE_PAGES=1 -pthread -DCPP_SUPPORT")
include_directories(include)
add_library(isoalloc SHARED src/iso_alloc_interfaces.c src/iso_alloc_stats.c src/iso_alloc.c src/iso_alloc.cpp src/malloc_hook.c)

