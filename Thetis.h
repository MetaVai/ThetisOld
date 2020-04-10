#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>

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

#define ThetisAssert(X) if (!(X)) { \
    thetis::error(__FILE__, __LINE__, "assert failed {}", #X); \
}

#define ThetisUse(x) (void) x
#define THETIS_MEM_MAGIC 0xab

namespace thetis {
#ifdef _DEBUG 
    template<typename T>
    inline T _check_canary(const T &x, const char * file, int line) {
        const unsigned char *b = reinterpret_cast<const unsigned char *>(&x);
        for (unsigned i=0; i< sizeof(T); ++i) {
            if (b[i] == THETIS_MEM_MAGIC) {
                thetis::error(file, line, "[check canary] assert failed ");
            }
        }
        return x;
    }
#define thetis_check_canary(x) thetis::_check_canary((x), __FILE__, __LINE__)

    bool _thetis_check_data(void *v);
    inline bool check_data(const void * v) {
        return _thetis_check_data(const_cast<void *>(v));
    }
#else
#define thetis_check_canary(X) (X)

    inline bool check_data(const void * v) { return true; }
#endif
}
