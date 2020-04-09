#include <omp.h>
#include "TestMatrix.h"

TEST_THETIS(TestMatrix01) {
    std::cout << "TestMatrix0101" << std::endl << std::flush;
    throw std::logic_error("bad bad");
}


TEST_THETIS(TestMatrix02) {
    spdlog::debug("test matrix 02 {}", 12345);
    CHECK(false);
}

TEST_THETIS(TestMatrix03) {
    spdlog::debug("test matrix 03 {}", 12345);
    THETIS_ASSERT(1==2);
}

TEST_THETIS(TestMatrix04) {
    spdlog::debug("test matrix 04 {}", 12345);
    spdlog::debug(" num threads = {}, max threads = {}", omp_get_num_threads(), omp_get_max_threads());
  
#pragma omp parallel for
    for (int i=0; i<16; ++i) {
        auto thr = omp_get_thread_num();
        spdlog::debug("thread #{}", thr);
    }
}
