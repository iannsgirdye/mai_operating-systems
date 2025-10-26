#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "../include/utilities.h"

#define OPEN_FAILURE -1

void reverse(char *str, ssize_t len) {
  if (len > 0 && str[len-1] == '\n') {
    len--;
  }
  
  for (ssize_t i = 0; i < len / 2; i++) {
    char tmp = str[i];
    str[i] = str[len - 1 - i];
    str[len - 1 - i] = tmp;
  }
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printError("Invalid command. Usage: ./child <filename>");
    exit(EXIT_FAILURE);
  }

  int file = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC, 0600);
  if (file == OPEN_FAILURE) {
    printError("Can not open the file.");
    exit(EXIT_FAILURE);
  }

  char buffer[BUFSIZ];
  ssize_t bufferLen;
  while ((bufferLen = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
    reverse(buffer, bufferLen);
    write(file, buffer, bufferLen);
  }

  close(file);
  return 0;
}
