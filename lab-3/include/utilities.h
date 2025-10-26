#ifndef UTILITIES_H
#define UTILITIES_H

#define GET_FILE_NAME_SUCCESS  0
#define GET_FILE_NAME_FAILURE -1
#define MIN_FILE_NAME_LENGTH   2

void printError(const char *message);

void print(const char *message);

int getFileName(char *fileName, int fileNumber);

#endif
