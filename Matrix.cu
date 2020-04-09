#include "Matrix.h"

inline int __host__ __device__ udiv(int a, int b) { return (a%b !=0)? (a/b+1) : (a/b); }

