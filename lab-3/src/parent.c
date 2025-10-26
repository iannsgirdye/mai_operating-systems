#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "../include/utilities.h"

#define PIPE_FAILURE           -1
#define IS_CHILD                0
#define FORK_FAILURE           -1
#define EXECV_FAILURE          -1
#define COMPLETE_CHILD_SUCCESS  0
#define COMPLETE_CHILD_FAILURE -1
#define READ_DATA_SUCCESS       0
#define READ_DATA_FAILURE      -1

int completeChild(const int pipe[], const int otherPipe[], char *fileName) {
  dup2(pipe[0], STDIN_FILENO);
  close(pipe[0]);
  close(pipe[1]);
  close(otherPipe[0]); 
  close(otherPipe[1]);

  const char *path = "./child";
  char *const argv[] = {"child", fileName, NULL};
  if (execv(path, argv) == EXECV_FAILURE) {
    return COMPLETE_CHILD_FAILURE;
  }
  return COMPLETE_CHILD_SUCCESS;
}

int isChild(pid_t child, int childNumber) {
  if (child == FORK_FAILURE) {
    char message[BUFSIZ];
    snprintf(message, BUFSIZ, "Invalid fork #%d.", childNumber);
    printError(message);
    exit(EXIT_FAILURE);
  }
  return child == IS_CHILD;
}

void startChild(int childNumber, const int pipe[], const int otherPipe[], char *fileName) {
  if (completeChild(pipe, otherPipe, fileName) == COMPLETE_CHILD_FAILURE) {
    char message[BUFSIZ];
    snprintf(message, BUFSIZ, "Invalid complete of the child #%d.", childNumber);
    printError(message);
    exit(EXIT_FAILURE);
  }
}

int readData(char buffer[], ssize_t *bufferLen) {
  *bufferLen = read(STDIN_FILENO, buffer, BUFSIZ);
  if (*bufferLen <= 0 || buffer[0] == '\n') {
    return READ_DATA_FAILURE;
  }
  return READ_DATA_SUCCESS;
}

void writeData(const char buffer[], ssize_t bufferLen, const int pipe1[], const int pipe2[]) {
  if ((rand() % 100) < 80) {
    write(pipe1[1], buffer, bufferLen);
    return;
  }
  write(pipe2[1], buffer, bufferLen);
}

int main() {
  int pipe1[2], pipe2[2];
  if (pipe(pipe1) == PIPE_FAILURE || pipe(pipe2) == PIPE_FAILURE) {
    printError("Pipe init error.");
    exit(EXIT_FAILURE);
  }

  char fileName1[BUFSIZ], fileName2[BUFSIZ];
  if (getFileName(fileName1, 1) == GET_FILE_NAME_FAILURE) {
    printError("Invalid name of the first file.");
    exit(EXIT_FAILURE);
  }
  if (getFileName(fileName2, 2) == GET_FILE_NAME_FAILURE) {
    printError("Invalid name of the second file.");
    exit(EXIT_FAILURE);
  }

  pid_t child1 = fork();
  if (isChild(child1, 1)) {
    startChild(1, pipe1, pipe2, fileName1);
  }

  pid_t child2 = fork();
  if (isChild(child2, 2)) {
    startChild(2, pipe2, pipe1, fileName2);
  }

  close(pipe1[0]);
  close(pipe2[0]);

  char buffer[BUFSIZ];
  ssize_t bufferLen;
  srand(time(NULL));
  while (readData(buffer, &bufferLen) == READ_DATA_SUCCESS) {
    writeData(buffer, bufferLen, pipe1, pipe2);
  }

  close(pipe1[1]);
  close(pipe2[1]);

  wait(NULL);
  wait(NULL);

  return 0;
}
