#include "../../include/utilities.h"
#include <unistd.h>
#include <string.h>

commandType checkCommand(int mode, int argsCount) {
  if (argsCount == 0) {
    return NO_COMMAND;
  }
  if (mode < 0 || mode > 2) {
    return INVALID_MODE;
  }
  int correctArgsCount[3] = {1, 3, 2};
  if (argsCount != correctArgsCount[mode]) {
    return INVALID_ARGS_COUNT;
  }
  return OK;
}

void printMessage(const char *message) {
  write(STDOUT_FILENO, message, strlen(message));
}

void printError(const char *error) {  
  write(STDERR_FILENO, error, strlen(error));
}
