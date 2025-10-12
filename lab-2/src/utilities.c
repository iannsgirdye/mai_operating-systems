#include "../include/utilities.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MIN_THREADS_COUNT 1
#define MAX_THREADS_COUNT 256

bool validateCommand(int argc, char *argv[]) {
  if (argc != 7) {
    return false;
  }
  if (strcmp(argv[1], "-l") != 0) {
    return false;
  }
  if (strcmp(argv[3], "-d") != 0) {
    return false;
  }
  if (strcmp(argv[5], "-t") != 0) {
    return false;
  }
  return true;
}

ssize_t parseArray(int array[], size_t length) {
  const size_t BIG_BUFSIZ = BUFSIZ * 128; 
  char buffer[BIG_BUFSIZ];
  const ssize_t bytesRead = read(STDIN_FILENO, buffer, BIG_BUFSIZ - 1);
  if (bytesRead < 1) {
    return -1;
  }
  buffer[bytesRead] = '\0';

  int n;
  int scanned = 0;
  size_t i = 0;
  while (i != length && scanned < bytesRead && sscanf(&buffer[scanned], "%d%n", &array[i], &n) == 1) {
    scanned += n;
    ++i;
  }

  return (ssize_t)i;
}

void copyArray(int src[], int dest[], size_t length) {
  for (size_t i = 0; i != length; ++i) {
    dest[i] = src[i];
  }
}

void printMessage(const char *message) {
  write(STDERR_FILENO, message, strlen(message)); 
}

void printInt(int number) {
  char strNumber[BUFSIZ];
  snprintf(strNumber, BUFSIZ, "%d", number);
  write(STDOUT_FILENO, strNumber, strlen(strNumber));
}

void printDouble(double number) {
  char strNumber[BUFSIZ];
  snprintf(strNumber, BUFSIZ, "%lf", number);
  write(STDOUT_FILENO, strNumber, strlen(strNumber));
}

void printArray(const char *sortType, const int array[], size_t length) {
  char buffer[BUFSIZ];

  snprintf(buffer, BUFSIZ, "./test/output_%s.txt", sortType);
  int fd = open(buffer, O_WRONLY | O_CREAT | O_TRUNC, 0644);

  for (size_t i = 0; i != length; ++i) {
    snprintf(buffer, BUFSIZ, "%d ", array[i]);
    write(fd, buffer, strlen(buffer));
  }
}
