#include "../include/utilities.h"
#include "../include/bitonicSort.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define DIRECTION_UP      1
#define DIRECTION_DOWN    0
#define MIN_ARRAY_LENGTH  1
#define MAX_ARRAY_LENGTH  100000
#define MIN_THREADS_COUNT 1
#define MAX_THREADS_COUNT 2048

double sequential(int *array1, size_t length) {
  clock_t start = clock();
  bitonicSortSequential(array1, length, DIRECTION_UP);
  clock_t end = clock();
  double time = (double)(end - start) / CLOCKS_PER_SEC;

  char message[BUFSIZ];
  snprintf(message, BUFSIZ, "  sequential:   %lf seconds;\n", time);
  printMessage(message);
  printArray("sequential", array1, length);

  return time;
}

double parallel(int *array2, size_t length, int threadsCount) {
  clock_t start = clock();
  bitonicSortParallel(array2, length, DIRECTION_UP, threadsCount);
  clock_t end = clock();
  double time = (double)(end - start) / CLOCKS_PER_SEC;

  char message[BUFSIZ];
  snprintf(message, BUFSIZ, "  parallel:     %lf seconds, %d threads;\n", time, getCurrentThreads());
  printMessage(message);
  printArray("parallel", array2, length);

  return time;
}

double acceleration(double sequentialTime, double parallelTime) {
  char message[BUFSIZ];
  double value = sequentialTime / parallelTime;
  snprintf(message, BUFSIZ, "  acceleration: %lf;\n", value);
  printMessage(message);

  return value;
}

void efficiency(double acceleration, int threadsCount) {
  char message[BUFSIZ];
  snprintf(message, BUFSIZ, "  efficiency:   %lf.\n", acceleration / threadsCount);
  printMessage(message);
}

int main(int argc, char *argv[]) {
  if (validateCommand(argc, argv) == false) {
    printMessage("Invalid command. Usage: ./a.out -l <length of array> -d <direction of sort> -t <count of threads>\n");
    exit(EXIT_FAILURE);
  }
  
  size_t length;
  sscanf(argv[2], "%zu", &length);
  if (length < MIN_ARRAY_LENGTH || length > MAX_ARRAY_LENGTH) {
    char message[BUFSIZ];
    sprintf(
      message,
      "Invalid length of array. It must be a natural number from %d to %d.\n",
      MIN_ARRAY_LENGTH, MAX_ARRAY_LENGTH
    );
    printMessage(message);
    exit(EXIT_FAILURE);
  }

  int direction;
  sscanf(argv[4], "%d", &direction);
  if (direction != DIRECTION_UP && direction != DIRECTION_DOWN) {
    printMessage("Invalid direction. It must be 0 or 1.\n");
    exit(EXIT_FAILURE);
  }

  int threadsCount;
  sscanf(argv[6], "%d", &threadsCount);
  if (threadsCount < MIN_THREADS_COUNT || threadsCount > MAX_THREADS_COUNT) {
    printMessage("Invalid count of threads. It must be natural value.\n");
    exit(EXIT_FAILURE);
  }

  int array1[MAX_ARRAY_LENGTH];
  ssize_t tmp = parseArray(array1, length);
  if (tmp != (ssize_t)length) {
    printInt((int)tmp);
    printMessage("Invalid array or file.\n");
    exit(EXIT_FAILURE);
  }

  int array2[MAX_ARRAY_LENGTH];
  copyArray(array1, array2, length);

  printMessage("Bitonic sort:\n");
  const double sequentialTime = sequential(array1, length);
  const double parallelTime = parallel(array2, length, threadsCount);
  const double accelerationValue = acceleration(sequentialTime, parallelTime);
  efficiency(accelerationValue, threadsCount);
  printMessage("Sorted arrays written to \"./test/output_sequential.txt\" and \"./test/output_parallel.txt\".\n");

  return 0;
}
