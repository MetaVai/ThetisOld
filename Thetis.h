#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <tuple>
#include <cassert>
#include <functional>
#include <thread>
#include <fstream>
#include <sstream>

#ifdef __linux__ 
#include <unistd.h>
#include <execinfo.h> // Linux only!
#else
#ifdef _WIN32
#include <Windows.h>

#define timegm _mkgmtime
inline void gmtime_r(const time_t* _t, struct tm* _tm) {
	*_tm = *gmtime(_t);
}
/*
gmtime_r(&now, &tm_time);
std::tm* gmtime( const std::time_t* time );

https://stackoverflow.com/questions/12044519/what-is-the-windows-equivalent-of-the-unix-function-gmtime-r
gmtime_r() is the thread-safe version of gmtime(). The MSVC implementation of gmtime() is already thread safe, the returned struct tm* is allocated in thread-local storage.
That doesn't make it immune from trouble if the function is called multiple times on the same thread and the returned pointer is stored. You can use gmtime_s() instead. Closest to gmtime_r() but with the arguments reversed ;)
*/

// alternative: https://stackoverflow.com/questions/16647819/timegm-cross-platform
/*
time_t timegm(struct tm * a_tm)
{
	time_t ltime = mktime(a_tm);
	struct tm tm_val;
	gmtime_s(&tm_val, &ltime);
	int offset = (tm_val.tm_hour - a_tm->tm_hour);
	if (offset > 12)
	{
		offset = 24 - offset;
	}
	time_t utc = mktime(a_tm) - offset * 3600;
	return utc;
}
*/

#else
#include <mach-o/dyld.h>
#endif
#endif

#ifdef _WIN32
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
#endif

// failure with CUDA compilter
#ifndef __CUDACC__
#include "external/spdlog/include/spdlog/spdlog.h"
#endif

#include "external/picojson/picojson.h"
#include "external/rapidxml/rapidxml.hpp"

namespace thetis {

    const char kPathSeparator =
#if defined _WIN32 || defined __CYGWIN__
    '\\';
#else
    '/';
#endif

    inline std::string directory_part(const std::string &filename) {
        std::string directory;
        const size_t last_slash_idx = filename.rfind(kPathSeparator);
        if (std::string::npos != last_slash_idx)
        {
            directory = filename.substr(0, last_slash_idx);
        }
        std::cerr << filename << " => " << directory << std::endl << std::flush;
        return directory;
    }

    inline std::string executable_path() {
       
#ifdef __unix__
 char buffer[1024] = {0};
        auto n = readlink("/proc/self/exe", buffer, sizeof(buffer));
        buffer[n] = 0;
        return directory_part(buffer);
#else
#ifdef _WIN32
		HMODULE hModule = GetModuleHandleA(NULL);
		char path[MAX_PATH];
		GetModuleFileNameA(hModule, path, MAX_PATH);
		return directory_part(path);
#else
char path[1024];
uint32_t size = sizeof(path);
if (_NSGetExecutablePath(path, &size) == 0)
    printf("executable path is %s\n", path);
else
    printf("buffer too small; need size %u\n", size);
//exit(1);
return directory_part(path);
#endif
#endif        
        
    }

    inline std::string readAllText(const std::string &s) {
        std::ifstream t(s);
        std::stringstream buffer;
        buffer << t.rdbuf();
        return buffer.str();
    }

    inline void writeAllText(const std::string &s, const std::string & txt) {
        std::ofstream t(s);
        t << txt;
        t.close();
    }

    /** Singleton */
    template<typename T, typename X>
    class _Singleton {
        static constexpr uint64_t magic = 0xcafecafefecafeca;
        static uint64_t nifty;
        static T* ptr;
        struct ResourceManager {
            inline T& get() {
                if (nifty != magic) {
                    nifty = magic;
                    ptr = new T();
                }
                return *ptr;
            }
            inline ~ ResourceManager() {
                if (nifty == magic) {
                    delete ptr;
                    ptr = nullptr;
                    nifty = 0;
                }
            }
        };
        static ResourceManager rm;
    public:
        static inline T& get() {
            return rm.get();
        }
    };
    template<typename T, typename X>
    uint64_t _Singleton<T,X>::nifty;
    template<typename T, typename X>
    T* _Singleton<T,X>::ptr;
    template<typename T, typename X>
    typename _Singleton<T,X>::ResourceManager _Singleton<T,X>::rm;
    template<typename T>
    using Singleton = _Singleton<T, int>;

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
