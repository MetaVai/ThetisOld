cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
option(USE_CUDA "CUDA/GPU project" ON)
option(USE_ISOALLOC "Isoalloc/GPU project" ON)

if (USE_CUDA)
    project(Thetis LANGUAGES CXX CUDA C)
else(USE_CUDA)
    project(Thetis LANGUAGES CXX C)
endif(USE_CUDA)

find_package(OpenMP REQUIRED)
set(CMAKE_CXX_STANDARD 17)
add_subdirectory(external/spdlog)

add_library(Thetis SHARED Matrix.cpp)
add_executable(TestThetis TestThetis.cpp TestMatrix.cpp doctest/parts/doctest.cpp)

target_link_libraries(TestThetis PUBLIC Thetis spdlog)
