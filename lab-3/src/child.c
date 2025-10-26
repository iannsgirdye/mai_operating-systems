#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <string.h>
#include "../include/shm.h"
#include "../include/utilities.h"

#define FD_FAILED  -1
#define SHM_FAILED -1

void freeBeforeMap(int shm_fd) {
  close(shm_fd);
}

void freeBeforeSemW(int shm_fd, shm_buffer_t *buf) {
  munmap(buf, sizeof(shm_buffer_t));
  freeBeforeMap(shm_fd);
}

void freeBeforeSemR(int shm_fd, shm_buffer_t *buf, sem_t *sem_w) {
  sem_close(sem_w);
  freeBeforeSemW(shm_fd, buf);
}

void freeBeforeFd(int shm_fd, shm_buffer_t *buf, sem_t *sem_w, sem_t *sem_r) {
  sem_close(sem_r);
  freeBeforeSemR(shm_fd, buf, sem_w);
}

void freeAll(int shm_fd, shm_buffer_t *buf, sem_t *sem_w, sem_t *sem_r, int fd) {
  close(fd);
  freeBeforeFd(shm_fd, buf, sem_w, sem_r);
}

void reverse(char *str, ssize_t len) {
  if (len > 0 && str[len - 1] == '\n') {
    len--;
  }
  
  for (ssize_t i = 0; i < len / 2; i++) {
    char tmp = str[i];
    str[i] = str[len - 1 - i];
    str[len - 1 - i] = tmp;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    printError("Invalid command. Usage: ./child <shm_name> <sem_w_name> <sem_r_name> <file_name>");
    exit(EXIT_FAILURE);
  }

  const char *shm_name = argv[1];
  const char *sem_w_name = argv[2];
  const char *sem_r_name = argv[3];
  const char *fileName = argv[4];

  int shm_fd = shm_open(shm_name, O_RDWR, 0600);
  if (shm_fd == SHM_FAILED) {
    printError("shm_open failed");
    exit(EXIT_FAILURE);
  }

  shm_buffer_t *buf = mmap(NULL, sizeof(shm_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (buf == MAP_FAILED) {
    freeBeforeMap(shm_fd);
    printError("mmap failed");
    exit(EXIT_FAILURE);
  }

  sem_t *sem_w = sem_open(sem_w_name, 0);
  if (sem_w == SEM_FAILED) {
    freeBeforeSemW(shm_fd, buf);
    printError("sem_open for write failed");
    exit(EXIT_FAILURE);
  }

  sem_t *sem_r = sem_open(sem_r_name, 0);
  if (sem_r == SEM_FAILED) {
    freeBeforeSemR(shm_fd, buf, sem_w);
    printError("sem_open for read failed");
    exit(EXIT_FAILURE);
  }

  int fd = open(fileName, O_CREAT | O_WRONLY | O_TRUNC, 0600);
  if (fd == FD_FAILED) {
    freeBeforeFd(shm_fd, buf, sem_w, sem_r);
    char message[BUFSIZ];
    snprintf(message, BUFSIZ, "Can not open the file \"%s\"", fileName);
    printError(message);
    exit(EXIT_FAILURE);
  }

  do {
    sem_wait(sem_r);

    if (buf->finished) {
      sem_post(sem_w);  
      break;
    }

    reverse(buf->data, buf->len);
    write(fd, buf->data, buf->len);

    sem_post(sem_w);
  } while (!buf->finished);

  freeAll(shm_fd, buf, sem_w, sem_r, fd);

  return 0;
}
