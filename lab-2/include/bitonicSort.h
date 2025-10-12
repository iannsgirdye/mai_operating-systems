#ifndef BITONIC_SORT
#define BITONIC_SORT

#include <stddef.h>

void bitonicSortSequential(int array[], size_t length, int direction);

void bitonicSortParallel(int array[], size_t length, int direction, int _maxThreads);

#endif
