#include "ThetisTest.h"
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#ifdef DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
DOCTEST_MSVC_SUPPRESS_WARNING_WITH_PUSH(4007) // 'function' : must be 'attribute' - see issue #182
int main(int argc, char** argv) { return doctest::Context(argc, argv).run(); }
DOCTEST_MSVC_SUPPRESS_WARNING_POP
#endif // DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

TEST_CASE("main") { std::cout << "hello from <main>" << std::endl << std::flush; }

// https://github.com/onqtam/doctest/blob/master/examples/range_based_execution.py ***
//  `./TestThetis -ltc | wc -l` "less" 4
//int main(int argc, const char* argv[]) {
//    return 0;
//}
