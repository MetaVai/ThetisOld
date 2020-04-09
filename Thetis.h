#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <tuple>

// failure with CUDA compilter
#ifndef __CUDACC__
#include "external/spdlog/include/spdlog/spdlog.h"
#endif

namespace thetis {
#ifndef __CUDACC__    
    template<typename ...A>
    inline void error(const char* file, int line, spdlog::string_view_t v, A... a) {
        // N.B. as this is an error, no need to over optimize...
        spdlog::critical("error @ file {}, line {}", file, line);
        spdlog::error(std::string("{}:{} ") + std::string(v.data()), file, line, a...);
        throw std::logic_error(file + std::string(":")+ std::to_string(line)); 
    }
#else
    template<typename ...A>
    inline void error(const char* file, int line, const char* v, A... a) {
        // N.B. as this is an error, no need to over optimize...
        std::cerr << "[CUDA] " << file << ":" << line << ": " << v << std::endl << std::flush;
        throw std::logic_error(file + std::string(":")+ std::to_string(line)); 
    }
#endif

}

#define THETIS_ASSERT(X) if (!(X)) { \
    thetis::error(__FILE__, __LINE__, "assert failed {}", #X); \
}

