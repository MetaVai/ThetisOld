cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
# N.B. this probably does not make much sense to try on a non Linux platform
project(Isoalloc LANGUAGES CXX C)
add_library(isoalloc SHARED src/iso_alloc_interfaces.c src/iso_alloc_stats.c src/iso_alloc.cpp src/malloc_hook.c)

