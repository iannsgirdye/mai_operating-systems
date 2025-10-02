#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "../include/errors.h"
#include "../include/colors.h"


#define PIPE_ERROR -1
#define IS_CHILD 0
#define FORK_ERROR -1
#define EXECV_ERROR -1
#define FILE_NAME_ERROR -1
#define FILE_NAME_SIZE 256
#define MESSAGE_SIZE 128
#define BUFFER_SIZE 1024
#define AFTER_FORK_ERROR -1
#define EMPTY_BUFFER -1


int getFileName(char* fileName, const int fileNumber) {
  char message[MESSAGE_SIZE];
  int messageLen = sprintf(
    message,
    COLOR_BOLD_CYAN "Enter name of the %d child file: " COLOR_WHITE, 
    fileNumber
  );
  write(STDOUT_FILENO, message, messageLen);
  
  ssize_t fileNameLen = read(STDIN_FILENO, fileName, sizeof(fileName) - 1);
  if (fileNameLen < 1) {
    errorInvalidFileName();
    return FILE_NAME_ERROR;
  }
  fileName[fileNameLen - 1] = '\0';
  return 0;
}


int afterFork(const int pipe1[], const int pipe2[], char *fileName) {
  dup2(pipe2[0], STDIN_FILENO);
  close(pipe2[0]); close(pipe2[1]);
  close(pipe1[0]); close(pipe1[1]);

  char *const args[] = {"child", fileName, NULL};
  if (execv("./child", args) == EXECV_ERROR) {
    errorExecv();
    return -1;
  }

  return 0;
}


int readData(char buffer[], ssize_t *bufferLen) {
  *bufferLen = read(STDIN_FILENO, buffer, BUFFER_SIZE);
  
  if (*bufferLen <= 0) {
    return -1;
  }
  
  if (buffer[0] == '\n') {
    return -1;
  }

  return 0;
}


void writeData(const char buffer[], const ssize_t bufferLen, const int pipe1[], const int pipe2[]) {
  if ((rand() % 100) < 80) {
    write(pipe1[1], buffer, bufferLen);
    return;
  }
  
  write(pipe2[1], buffer, bufferLen);
}


int main() {
  int pipe1[2], pipe2[2];
  if (pipe(pipe1) == PIPE_ERROR || pipe(pipe2) == PIPE_ERROR) {
    errorPipe();
    return 0;
  }

  char fileName1[FILE_NAME_SIZE], fileName2[FILE_NAME_SIZE];
  if (getFileName(fileName1, 1) == FILE_NAME_ERROR) {
    return 0;
  }
  if (getFileName(fileName2, 2) == FILE_NAME_ERROR) {
    return 0;
  }

  pid_t child1 = fork();
  switch (child1) {
    case FORK_ERROR: {
      errorFork();
      return 0;
    }
    case IS_CHILD: {
      if (afterFork(pipe1, pipe2, fileName1) == AFTER_FORK_ERROR) {
        return 0;
      }
    }
  }

  pid_t child2 = fork();
  switch (child2) {
    case FORK_ERROR: {
      errorFork();
      return 0;
    }
    case IS_CHILD: {
      if (afterFork(pipe1, pipe2, fileName2) == AFTER_FORK_ERROR) {
        return 0;
      }
    }
  }

  close(pipe1[0]);
  close(pipe2[0]);

  char buffer[BUFFER_SIZE];
  ssize_t bufferLen;
  srand(time(NULL));
  while (true) {
    if (readData(buffer, &bufferLen) == EMPTY_BUFFER) {
      break;
    }
    
    writeData(buffer, bufferLen, pipe1, pipe2);
  }

  close(pipe1[1]);
  close(pipe2[1]);

  wait(NULL);
  wait(NULL);

  return 0;
}
