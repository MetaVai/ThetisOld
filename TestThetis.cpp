#include <omp.h>
#include "ThetisTest.h"

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4007) // 'function' : must be 'attribute' - see issue #182
int main(int argc, char** argv) { 
    omp_set_dynamic(1);
    omp_set_nested(1); 
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug(" num threads = {}, max threads = {}", omp_get_num_threads(), omp_get_max_threads());

    auto rv = doctest::Context(argc, argv).run(); 
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
