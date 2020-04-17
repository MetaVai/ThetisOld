#include "TestMatrix.h"

#ifdef BOOM
TEST_THETIS(TestMatrixCuda01) {
   // spdlog::debug("TestMatrixCuda01"); not supported ***
   ThetisAssert(false);
}
#endif 

#ifdef BOOM
TEST_THETIS(TestMatrixCuda05) {
   auto *data = new int[45];
   int acc = 0;
   for(int i=0; i<46; ++i) {
       acc += thetis_check_canary(data[i]); // -> thetis_alloc
   }
   std::cerr << acc << std::endl;
   delete[](data);
   //delete[](data); // caught/iso alloc
}
#endif

#ifdef BOOM
TEST_THETIS(TestMatrixCuda06) {
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

TEST_THETIS_PARALLEL(TestMatrixCuda07) {
   auto *data = new int[45];
   int acc = 0;
   for(int i=0; i< 45 /*46*/; ++i) { // Confirmed BOOM is 46
       data[i] = i+4;
   }
   std::cerr << acc << std::endl;
   delete[](data);   
}
