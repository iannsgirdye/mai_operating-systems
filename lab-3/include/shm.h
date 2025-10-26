#ifndef SHM_H
#define SHM_H

#include <unistd.h>
#include <stdio.h>

typedef struct {
  ssize_t len;
  char data[BUFSIZ];
  int finished;
} shm_buffer_t;

#endif
