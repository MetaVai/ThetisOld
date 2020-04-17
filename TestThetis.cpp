#include <omp.h>
#include "ThetisTest.h"

#define CPP_SUPPORT 1
#include "external/isoalloc/include/iso_alloc.h"
#include "external/ThreadPool/ThreadPool.h"

namespace thetis {
    /** When using the malloc wrapper, convert wrapped chunk size to actual size */
    inline size_t thetis_real_alloc_size(size_t s) { return s+ 4096*2; }
    /** When using the malloc wrapper, convert actual alloc-ed pointer to wrapped pointer */
    inline void* thetis_real_pointer(void * v) { return (void*)(((uint8_t*)v)+4096); }
    /** When using the malloc wrapper, convert the wrapped pointer to actual alloc-ed pointer */
    inline void* thetis_from_real_pointer(void * v) { return (void*)(((uint8_t*)v)-4096); }
    /** When using the malloc wrapper, initialize guard bytes */
    inline void thetis_initialize_data(void *v, size_t size) {
        auto v2 = thetis_from_real_pointer(v); 
        ThetisUse(v2);
        ThetisUse(size);
        *((size_t*)v2) = size;
        for (uint8_t* p = ((uint8_t *)v2) + sizeof(size_t); p < (uint8_t*) v; ++p) {
            *p = THETIS_MEM_MAGIC;
        }
        for (uint8_t* p = ((uint8_t*)v)+size; p < ((uint8_t*)v)+size+4096; ++p) {
            *p = THETIS_MEM_MAGIC;
        }           
    }
    /** Check data for out-of-boundary writes 
     * @param v: buffer to check
     * @param Probably_no_what_you_think: set to true if this buffer was probably not allocated via the wrapped malloc...
     */
    inline bool thetis_check_data(void *v, bool &probably_not_what_you_think) {
        auto v2 = thetis_from_real_pointer(v);
        auto size = *((size_t*)v2);

        int c = 0;
        for (uint8_t* p = ((uint8_t *)v2) + sizeof(size_t); p < (uint8_t*) v; ++p) {
            if (!(*p == THETIS_MEM_MAGIC)) {
                if (c < 10) {
                    probably_not_what_you_think = true;
                    return true;
                }

                fprintf(stderr, "thetis_check_data failure size=%ld [1]\n", size);
                fflush(stderr);  
                return false;
            }

            ++ c;
        }
        for (uint8_t* p = ((uint8_t*)v)+size; p < ((uint8_t*)v)+size+4096; ++p) {
            if (!(*p == THETIS_MEM_MAGIC)) {
                fprintf(stderr, "thetis_check_data failure size=%ld [2] %ld\n", size, (int64_t)(p - ((uint8_t*)v)));
                fflush(stderr);
                return false;
            }
        }

        return true;
    }

    bool _thetis_check_data(void *v) {
        bool pnwyth = false;
        return pnwyth || thetis_check_data(v, pnwyth);
    }

    inline void* thetis_alloc(std::size_t size) {
        auto s2 = thetis::thetis_real_alloc_size(size);
        auto p2 = malloc(s2);
        auto p = thetis::thetis_real_pointer(p2);
        thetis::thetis_initialize_data(p, size);
        return p;
    }

    inline void thetis_free(void * x) {
        auto p2 = thetis::thetis_from_real_pointer(x);
        bool pnwyt = false;
        assert(thetis::thetis_check_data(x, pnwyt));
        if (pnwyt) {
            fprintf(stderr, "pnwyt\n");
            fflush(stderr);
            free(x);
        } else {
            free(p2);
        }
    }
}

#if defined(_DEBUG) && !defined(THETIS_CPU_ONLY)
#if defined(THETIS_USE_ISO_ALLOC)
void* operator new (std::size_t size) throw (/*std::bad_alloc*/) {
    return thetis::thetis_alloc(size);
}

void operator delete(void* x) throw() {   
    thetis::thetis_free(x);
    iso_verify_zones();
    int32_t r = iso_alloc_detect_leaks();
    if (r > 0) std::cerr << "[[FATAL]]" << r << " leaks " << std::endl << std::flush;
}
#else
void* operator new (std::size_t size) throw (/*std::bad_alloc*/) {
    return iso_alloc(size);
}

void operator delete(void* x) throw() {   
    iso_free(x);
    iso_verify_zones();
    int32_t r = iso_alloc_detect_leaks();
    if (r > 0) std::cerr << "[[FATAL]]" << r << " leaks " << std::endl << std::flush;
}
#endif
#endif

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4007) // 'function' : must be 'attribute' - see issue #182
int main(int argc, char** argv) { 
    omp_set_dynamic(1);
    omp_set_nested(1); 
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug(" num threads = {}, max threads = {}", omp_get_num_threads(), omp_get_max_threads());

    auto rv = doctest::Context(argc, argv).run(); 

    spdlog::debug("waiting for parallel tests");
    ThetisAssert(thetis::check_tests());
    spdlog::debug("parallel tests 'probably' passed");
    spdlog::shutdown();
    return rv;
}
DOCTEST_MSVC_SUPPRESS_WARNING_POP
#endif // DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

TEST_CASE("main") { std::cout << "hello from <main>" << std::endl << std::flush; }

// https://github.com/onqtam/doctest/blob/master/examples/range_based_execution.py ***
//  `./TestThetis -ltc | wc -l` "less" 4
//int main(int argc, const char* argv[]) {
//    return 0;
//}

namespace {
    thread_pool::ThreadPool pool(omp_get_max_threads());
    std::vector< std::future<bool> > results;
    std::vector< std::string > names;  
}

namespace thetis {
    void start_test(const std::string &name, std::function<bool()> f) {
        results.emplace_back(pool.enqueue(f));
        names.emplace_back(name);
    }

    bool check_tests() {
        bool status = true;
        for (unsigned i=0; i<results.size(); ++i) {
            if (!results[i].get()) {
                status = false;
                spdlog::error("// test failed: {}", names[i]);
            } else {
                spdlog::debug("// test success: {}", names[i]);
            }
        }

        ThetisAssert(status);
        return status;
    }
}
