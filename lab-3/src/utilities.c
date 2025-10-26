#include "../include/utilities.h"
#include <string.h>
#include <unistd.h>

void printError(const char *message) {
  write(STDERR_FILENO, message, strlen(message));
}
