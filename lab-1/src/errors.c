#include "../include/errors.h"
#include "../include/colors.h"
#include <stdlib.h>
#include <unistd.h>


void errorPipe() {
  const char *message = COLOR_BOLD_RED "error: " COLOR_WHITE "pipe failed\n";
  write(STDERR_FILENO, message, sizeof(message));
  exit(EXIT_FAILURE);
}


void errorFork() {
  const char *message = COLOR_BOLD_RED "error: " COLOR_WHITE "fork failed\n";
  write(STDERR_FILENO, message, sizeof(message));
  exit(EXIT_FAILURE);
}


void errorExecv() {
  const char *message = COLOR_BOLD_RED "error: " COLOR_WHITE "exec child\n";
  write(STDERR_FILENO, message, sizeof(message));
  exit(EXIT_FAILURE);
}


void errorInvalidCountOfAgruments() {
  const char *message = COLOR_BOLD_CYAN "usage: " COLOR_WHITE "child filename\n";
  write(STDERR_FILENO, message, sizeof(message));
  exit(EXIT_FAILURE);
}


void errorOpenFile() {
  const char *message = COLOR_BOLD_CYAN "error:" COLOR_WHITE "cannot open file\n";
  write(STDERR_FILENO, message, sizeof(message));
  exit(EXIT_FAILURE);
}
