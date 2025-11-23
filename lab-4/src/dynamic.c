#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dlfcn.h>

#include "../include/contracts.h"
#include "../include/utilities.h"

#define BUFFER_SIZE 128

typedef int prime_count_func(int, int);
typedef float pi_func(int);

static prime_count_func *prime_count_ptr = NULL;
static pi_func *pi_ptr = NULL;
static void *library = NULL;

static int current_implementation = 1;

static void load_library() {
  if (library) {
    dlclose(library);
  }

  const char *libraryName = (current_implementation == 1) ? "./lib1.so" : "./lib2.so";
  library = dlopen(libraryName, RTLD_LOCAL | RTLD_NOW);
  if (library == NULL) {
    printError("Can't load the library\n");
    exit(EXIT_FAILURE);
  }

  prime_count_ptr = dlsym(library, "prime_count");
  if (prime_count_ptr == NULL) {
    printError("Can't find the function \"prime_count\"\n");
    exit(EXIT_FAILURE);
  }

  pi_ptr = dlsym(library, "pi");
  if (pi_ptr == NULL) {
    printError("Can't find the function \"pi\"\n");
    exit(EXIT_FAILURE);
  }
}

int main() {
  printMessage(
    "0               - switch implementation of functions\n"
    "1 <start> <end> - calculate count of prime numbers in range [start; end]\n"
    "2 <accuracy>    - calculate pi\n\n"
  );

  load_library();

  char buffer[BUFFER_SIZE];
  int mode = 0, arg1 = 0, arg2 = 0, argsCount;
  ssize_t len;

  while ((len = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1)) > 0) {
    buffer[len] = '\0';
    argsCount = sscanf(buffer, "%d%d%d", &mode, &arg1, &arg2);

    switch(checkCommand(mode, argsCount)) {
      case NO_COMMAND: printError("Invalid input\n"); break;
      case INVALID_MODE: printError("Invalid mode\n"); break;
      case INVALID_ARGS_COUNT: printError("Invalid count of arguments\n"); break;
      case OK:
        switch (mode) {
          case 0:
            current_implementation = (current_implementation == 1) ? 2 : 1;
            load_library();
            snprintf(buffer, BUFFER_SIZE, "Implementation switched to library #%d\n", current_implementation);
            printMessage(buffer);
            break;
          case 1:
            snprintf(buffer, BUFFER_SIZE, "Result from library #%d: %d\n", current_implementation, prime_count_ptr(arg1, arg2));
            printMessage(buffer);
            break;
          case 2:
            snprintf(buffer, BUFFER_SIZE, "Result from library #%d: %f\n", current_implementation, pi_ptr(arg1));
            printMessage(buffer);
            break;
        }
    }
  }

  if (library) {
    dlclose(library);
  }

  return 0;
}
