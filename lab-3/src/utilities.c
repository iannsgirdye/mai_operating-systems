#include "../include/utilities.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void printError(const char *message) {
  write(STDERR_FILENO, message, strlen(message));
}

void print(const char *message) {
  write(STDOUT_FILENO, message, strlen(message));
}

int getFileName(char *fileName, int fileNumber) {
  char message[BUFSIZ];
  int messageLen = snprintf(message, BUFSIZ, "Enter the name of the child file #%d: ", fileNumber);
  write(STDOUT_FILENO, message, messageLen);
  
  ssize_t fileNameLen = read(STDIN_FILENO, fileName, BUFSIZ - 1);
  if (fileNameLen < MIN_FILE_NAME_LENGTH) {
    printError("Invalid name of the file.");
    return GET_FILE_NAME_FAILURE;
  }
  fileName[fileNameLen - 1] = '\0';
  return GET_FILE_NAME_SUCCESS;
}
