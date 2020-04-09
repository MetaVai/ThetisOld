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
