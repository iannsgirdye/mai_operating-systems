#ifndef UTILITIES_H
#define UTILITIES_H

typedef enum {
  OK,
  NO_COMMAND,
  INVALID_MODE,
  INVALID_ARGS_COUNT
} commandType;

commandType checkCommand(int mode, int argsCount);

void printMessage(const char *message);
void printError(const char *error);

#endif
