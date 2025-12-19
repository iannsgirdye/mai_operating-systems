#include "../include/bitonicSort.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>

void compareAndSwap(int array[], size_t i, size_t j, int direction) {
  if ((array[i] > array[j]) == direction) {
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
  }
}

void bitonicMergeSequential(int array[], size_t start, size_t length, int direction) {
  if (length < 2) {
    return;
  }
  
  size_t middle = length / 2;
  for (size_t i = start; i != start + middle; i++) {
    compareAndSwap(array, i, i + middle, direction);
  }
  bitonicMergeSequential(array, start, middle, direction);
  bitonicMergeSequential(array, start + middle, middle, direction);
}

void bitonicRecursiveSortSequential(int array[], size_t start, size_t length, int direction) {
  if (length < 2) {
    return;
  }
  
  size_t middle = length / 2;
  bitonicRecursiveSortSequential(array, start, middle, direction);
  bitonicRecursiveSortSequential(array, start + middle, middle, !direction);
  bitonicMergeSequential(array, start, length, direction);
}

void bitonicSortSequential(int array[], size_t lenght, int direction) {
  if (array == NULL) {
    return;
  }
  bitonicRecursiveSortSequential(array, 0, lenght, direction);
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int currentThreads;
int maxThreads;

typedef struct {
  int *array;
  size_t start;
  size_t length;
  int direction;
} ThreadArgs;

void *bitonicRecursiveSortParallel(void *args);

void bitonicMergeParallel(int array[], size_t start, size_t length, int direction) {
  if (length < 2) {
    return;
  }

  size_t middle = length / 2;

  for (size_t i = start; i < start + middle; i++) {
    compareAndSwap(array, i, i + middle, direction);
  }

  pthread_mutex_lock(&mutex);
  bool doParallel = (currentThreads + 2 <= maxThreads) ? true : false;
  if (doParallel) {
    currentThreads += 2;
  }
  pthread_mutex_unlock(&mutex);
  if (!doParallel) {
    bitonicMergeSequential(array, start, middle, direction);
    bitonicMergeSequential(array, start + middle, middle, direction);
    return;
  }

  pthread_t thread1;
  ThreadArgs *args1 = (ThreadArgs *)malloc(sizeof(ThreadArgs));
  args1->array = array;
  args1->start = start;
  args1->length = middle;
  args1->direction = direction;
  pthread_create(&thread1, NULL, bitonicRecursiveSortParallel, args1);
  
  pthread_t thread2;
  ThreadArgs *args2 = (ThreadArgs *)malloc(sizeof(ThreadArgs));
  args2->array = array;
  args2->start = start + middle;
  args2->length = middle;
  args2->direction = !direction;
  pthread_create(&thread2, NULL, bitonicRecursiveSortParallel, args2);  
  
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  
  bitonicMergeParallel(array, start, length, direction);
}

void *bitonicRecursiveSortParallel(void *args) {
  ThreadArgs *_args = (ThreadArgs *)args;
  int *array = _args->array;
  size_t start = _args->start;
  size_t length = _args->length;
  int direction = _args->direction;

  if (length < 2) {
    return NULL;
  }

  size_t middle = length / 2;

  pthread_mutex_lock(&mutex);
  bool doParallel = (currentThreads + 2 <= maxThreads) ? true : false;
  if (doParallel) {
    currentThreads += 2;
  }
  pthread_mutex_unlock(&mutex);
  if (!doParallel) {
    bitonicRecursiveSortSequential(array, start, middle, direction);
    bitonicRecursiveSortSequential(array, start + middle, middle, !direction);
    bitonicMergeSequential(array, start, length, direction);
    return NULL;
  }

  pthread_t thread1;
  ThreadArgs args1 = {.array = array, .start = start, .length = middle, .direction = direction};
  pthread_create(&thread1, NULL, bitonicRecursiveSortParallel, &args1);
  
  pthread_t thread2;
  ThreadArgs args2 = {.array = array, .start = start + middle, .length = middle, .direction = !direction};
  pthread_create(&thread2, NULL, bitonicRecursiveSortParallel, &args2);  
  
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
  
  bitonicMergeParallel(array, start, length, direction);

  pthread_mutex_lock(&mutex);
  currentThreads -= 2;
  pthread_mutex_unlock(&mutex);

  return NULL;
}

void bitonicSortParallel(int array[], size_t length, int direction, int _maxThreads) {
  currentThreads = 0;
  maxThreads = _maxThreads;

  ThreadArgs args = {.array = array, .start = 0, .length = length, .direction = direction};
  pthread_t mainThread;
  pthread_create(&mainThread, NULL, bitonicRecursiveSortParallel, &args);
  pthread_join(mainThread, NULL);
}
