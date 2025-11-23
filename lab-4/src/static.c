#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/contracts.h"
#include "../include/utilities.h"

#define BUFFER_SIZE 128

int main() {
  printMessage(
    "1 <start> <end> - calculate count of prime numbers in range [start; end]\n"
    "2 <accuracy>    - calculate PI\n\n"
  );

  char buffer[BUFFER_SIZE];
  int mode = 0;
  int arg1 = 0;
  int arg2 = 0;
  ssize_t len;
  int argsCount;

  while ((len = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1)) > 0) {
    buffer[len] = '\0';
    argsCount = sscanf(buffer, "%d%d%d", &mode, &arg1, &arg2);
    
    switch(checkCommand(mode, argsCount)) {
      case NO_COMMAND: printError("Invalid input\n"); break;
      case INVALID_MODE: printError("Invalid mode\n"); break;
      case INVALID_ARGS_COUNT: printError("Invalid count of arguments\n"); break;
      case OK:
        switch (mode) {
          case 1:
            snprintf(buffer, BUFFER_SIZE, "%d\n", prime_count(arg1, arg2));
            write(STDOUT_FILENO, buffer, strlen(buffer));
            break;
          case 2:
            snprintf(buffer, BUFFER_SIZE, "%f\n", pi(arg1));
            write(STDOUT_FILENO, buffer, strlen(buffer));
            break;
        }
    }
  }

  return 0;
}
