#include <omp.h>
#include "TestMatrix.h"

#ifdef BOOM
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
    ThetisAssert(1==2);
}
#endif 

TEST_THETIS(TestMatrix04) {
    spdlog::debug("test matrix 04 {}", 12345);
    spdlog::debug(" num threads = {}, max threads = {}", omp_get_num_threads(), omp_get_max_threads());
  
#pragma omp parallel for
    for (int i=0; i<16; ++i) {
        auto thr = omp_get_thread_num();
        spdlog::debug("thread #{}", thr);
    }
}

#ifdef BOOM
TEST_THETIS(TestMatrix05) {
    spdlog::debug("test matrix 05 {}", 12345);
    auto *data = new int[45];
    int acc = 0;
    for(int i=0; i<46; ++i) {
        acc += thetis_check_canary(data[i]); // caught by address sanitizer & thetis_alloc
    }
    std::cerr << acc << std::endl;
    delete[](data);
    //delete[](data); // caught/iso alloc
}

TEST_THETIS(TestMatrix06) {
    spdlog::debug("test matrix 05 {}", 12345);
    auto *data = new int[45];
    int acc = 0;
    for(int i=0; i<46; ++i) {
        data[i] = i+4;
    }
    std::cerr << acc << std::endl;
    delete[](data);
    //delete[](data); // caught/iso alloc
}
#endif

TEST_THETIS_PARALLEL(TestMatrix07) {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    // ThetisAssert(1==2);
}

TEST_THETIS_PARALLEL(TestMatrix08) {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    ThetisAssert(1==1);
}
