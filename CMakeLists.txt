cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
option(USE_CUDA "CUDA/GPU project" ON)
option(USE_ISOALLOC "Isoalloc/GPU project" OFF) # if OFF, use thetis_alloc ~~~
option(USE_JULIA "Julia script support" ON)

message("System = ${CMAKE_SYSTEM_NAME}, UNIX = ${UNIX}")

if (USE_CUDA)
    project(Thetis LANGUAGES CXX CUDA C)
else(USE_CUDA)
    project(Thetis LANGUAGES CXX C)
endif(USE_CUDA)

if (USE_JULIA)
  #cf. https://github.com/barche/embedding-julia 
  set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
  find_package(Julia REQUIRED)
endif(USE_JULIA)

find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
 message("using openmp")
 set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/spdlog)
include_directories(external/spdlog/include)
include_directories(external/picojson) # 'dom' json
include_directories(external/agency)
include_directories(/usr/local/cuda/include)
include_directories("$ENV{HOME}/libs/include")
link_directories("$ENV{HOME}/libs/lib")

if(USE_ISOALLOC)
    include_directories(external/isoalloc/include)
    add_subdirectory(external/isoalloc)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTHETIS_USE_ISO_ALLOC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTHETIS_USE_ISO_ALLOC")
    if(USE_CUDA)
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS}  -DTHETIS_USE_ISO_ALLOC")
    endif(USE_CUDA)
endif(USE_ISOALLOC)

add_library(Thetis SHARED Matrix.cpp)
if(USE_CUDA)
    add_library(ThetisGPU SHARED Matrix.cpp Matrix.cu)
    set_target_properties(ThetisGPU PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif(USE_CUDA)

add_executable(TestThetis TestThetis.cpp TestMatrix.cpp TestUtils.cpp TestJulia.cpp doctest/parts/doctest.cpp)

if(USE_CUDA)
    add_executable(TestThetisGPU TestThetis.cpp TestThetis.cu TestMatrix.cpp TestMatrix.cu TestUtils.cpp TestAgency.cu TestJulia.cpp doctest/parts/doctest.cpp)
    set_target_properties(TestThetisGPU PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif(USE_CUDA)

if (UNIX)
    message("Linux/Unix build!")

    if(USE_JULIA)
        target_include_directories(TestThetis PUBLIC
            "$<BUILD_INTERFACE:${Julia_INCLUDE_DIRS}>"
        )
        target_include_directories(TestThetisGPU PUBLIC
        "$<BUILD_INTERFACE:${Julia_INCLUDE_DIRS}>"
        )
        add_definitions(-DJULIA_ENABLE_THREADING -DENABLE_JULIA)
    else(USE_JULIA)

    endif(USE_JULIA)

    message("max error level")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -fPIC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -fPIC")
    if (USE_CUDA)
        include_directories(/usr/local/cuda/include)
        set(CMAKE_CUDA_FLAGS "${CMAKE_CUDA_FLAGS} -arch=sm_61 --expt-extended-lambda -Xcompiler -fPIC")
    endif(USE_CUDA)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("CMAKE_BUILD_TYPE = '' => -g -O0 -D_DEBUG=1")

        target_compile_options(TestThetisGPU PUBLIC "-g" "-O0" "-D_DEBUG=1" )
        target_compile_options(ThetisGPU PUBLIC "-g" "-O0" "-D_DEBUG=1" )
        target_compile_options(TestThetis PUBLIC "-g" "-rdynamic" "-O0" "-D_DEBUG=1" "-fsanitize=address" "-fsanitize=leak" "-DTHETIS_CPU_ONLY=1")
        target_compile_options(Thetis PUBLIC "-g" "-rdynamic" "-O0" "-D_DEBUG=1" "-fsanitize=address" "-fsanitize=leak" "-DTHETIS_CPU_ONLY=1")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -rdynamic -O0 -D_DEBUG=1")
        message(" please use -DCMAKE_BUILD_TYPE=Release or -DCMAKE_BUILD_TYPE=Debug if needed ")
    else(CMAKE_BUILD_TYPE STREQUAL "Debug")
        message("CMAKE_BUILD_TYPE defined as *" ${CMAKE_BUILD_TYPE} "*")
    endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        message("RELEASE COMPILE FLAGS ON: OPTIMIZE FOR SPEED WITH NATIVE(?) ARCH")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -rdynamic -O2")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -rdynamic -O2")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -rdynamic -O2")
    endif(CMAKE_BUILD_TYPE STREQUAL "Release")

    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_libraries(TestThetis PUBLIC Thetis spdlog $<BUILD_INTERFACE:${Julia_LIBRARY}> ${OpenMP_CXX_LIBRARIES})
    else (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_link_libraries(TestThetis PUBLIC Thetis spdlog $<BUILD_INTERFACE:${Julia_LIBRARY}> ${OpenMP_CXX_LIBRARIES} "-fsanitize=address" "-fsanitize=leak")
    endif (CMAKE_BUILD_TYPE STREQUAL "Release")

    if(USE_CUDA)
        if(USE_ISOALLOC)
            target_link_libraries(TestThetisGPU PUBLIC ThetisGPU spdlog isoalloc $<BUILD_INTERFACE:${Julia_LIBRARY}> ${OpenMP_CXX_LIBRARIES} -lcudart)    
        else(USE_ISOALLOC)
            target_link_libraries(TestThetisGPU PUBLIC ThetisGPU spdlog $<BUILD_INTERFACE:${Julia_LIBRARY}> ${OpenMP_CXX_LIBRARIES} -lcudart)
        endif(USE_ISOALLOC)
    endif(USE_CUDA)
endif(UNIX)
