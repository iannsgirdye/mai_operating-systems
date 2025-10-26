#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>
#include "../include/shm.h"
#include "../include/utilities.h"

#define IPC_NAME_LEN 64

typedef struct {
  int shm_fd;
  shm_buffer_t *buf;
  sem_t *sem_w;
  sem_t *sem_r;
  char shm_name[IPC_NAME_LEN];
  char sem_w_name[IPC_NAME_LEN];
  char sem_r_name[IPC_NAME_LEN];
} channel_t;
