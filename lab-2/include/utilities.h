#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

bool validateCommand(int argc, char *argv[]);

ssize_t parseArray(int array[], size_t length);

void copyArray(int src[], int dest[], size_t length);

void printMessage(const char *message);

void printInt(int number);

void printDouble(double number);

void printArray(const char *sortType, const int array[], size_t length);

#endif
