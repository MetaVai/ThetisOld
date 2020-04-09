#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <tuple>

#include "external/spdlog/include/spdlog/spdlog.h"

namespace thetis {
    template<typename ...A>
    inline void error(const char* file, int line, spdlog::string_view_t v, A... a) {
        // N.B. as this is an error, no need to over optimize...
        spdlog::critical("error @ file {}, line {}", file, line);
        spdlog::error(std::string("{}:{} ") + std::string(v.data()), file, line, a...);
        throw std::logic_error(file + std::string(":")+ std::to_string(line)); 
    }

}

#define THETIS_ASSERT(X) if (!(X)) { \
    thetis::error(__FILE__, __LINE__, "assert failed {}", #X); \
}

