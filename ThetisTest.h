#pragma once
#include "Thetis.h"
#include "doctest/doctest.h"
#define TEST_THETIS(X) TEST_CASE(#X)
#define TEST_THETIS_PARALLEL(X) \
    void _TEST_CASE_PARALLEL_ ##X (); \
    TEST_CASE(#X) { \
        thetis::start_test(#X, [&]() -> bool { \
            try { \
                _TEST_CASE_PARALLEL_ ##X (); \
            } catch(std::exception &e) { \
                std::cerr << "[TEST ERROR] " << #X << " " << __FILE__ << ":" << __LINE__ << std::endl << std::flush; \
                return false; \
            } \
            return true; \
        }); \
    } \
    void _TEST_CASE_PARALLEL_ ##X ()

namespace thetis {
    void start_test(const std::string &name, std::function<bool()> f);
    bool check_tests();
}
