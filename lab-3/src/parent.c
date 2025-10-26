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

#define FORK_FAILURE   -1
#define FORK_CHILD      0
#define IPC_NAME_LEN   64
#define CHILDREN_COUNT  2

typedef struct {
  int shm_fd;
  shm_buffer_t *buf;
  sem_t *sem_w;
  sem_t *sem_r;
  char shm_name[IPC_NAME_LEN];
  char sem_w_name[IPC_NAME_LEN];
  char sem_r_name[IPC_NAME_LEN];
} channel_t;

void initСhannel(channel_t *ch, size_t i, pid_t pid) {
  snprintf(ch->shm_name, IPC_NAME_LEN, "/shm_%d_%zu", pid, i);
  snprintf(ch->sem_w_name, IPC_NAME_LEN, "/sem_w_%d_%zu", pid, i);
  snprintf(ch->sem_r_name, IPC_NAME_LEN, "/sem_r_%d_%zu", pid, i);

  ch->shm_fd = shm_open(ch->shm_name, O_CREAT | O_RDWR, 0600);

  ftruncate(ch->shm_fd, sizeof(shm_buffer_t));

  ch->buf = mmap(NULL, sizeof(shm_buffer_t), PROT_READ | PROT_WRITE, MAP_SHARED, ch->shm_fd, 0);

  ch->sem_w = sem_open(ch->sem_w_name, O_CREAT, 0600, 1);
  ch->sem_r = sem_open(ch->sem_r_name, O_CREAT, 0600, 0);

  ch->buf->finished = 0;
}

int main() {
  srand(time(NULL));
  pid_t pid = getpid();

  channel_t channels[CHILDREN_COUNT];
  char fileNames[CHILDREN_COUNT][BUFSIZ];
  for (size_t i = 0; i != CHILDREN_COUNT; ++i) {
    if (getFileName(fileNames[i], i + 1) == GET_FILE_NAME_FAILURE) {
      printError("Invalid file name.");
      exit(EXIT_FAILURE);
    }
    initСhannel(&channels[i], i, pid);
  }

  const char *path = "./child";
  const char *program = "child";
  for (size_t i = 0; i != CHILDREN_COUNT; ++i) {
    switch (fork()) {
      case FORK_FAILURE:
        printError("Fork failed.");
        exit(EXIT_FAILURE);
      case FORK_CHILD:
        execl(path, program, channels[i].shm_name, channels[i].sem_w_name, channels[i].sem_r_name, fileNames[i], NULL);
        exit(EXIT_FAILURE);
    }
  }

  char buf[BUFSIZ];
  ssize_t len;
  while ((len = read(STDIN_FILENO, buf, BUFSIZ)) > 0) {
    size_t i = (rand() % 100 < 80) ? 0 : 1;
    channel_t *ch = &channels[i];

    sem_wait(ch->sem_w);

    memcpy(ch->buf->data, buf, len);
    ch->buf->len = len;

    sem_post(ch->sem_r);
  }

  for (size_t i = 0; i != CHILDREN_COUNT; ++i) {
    sem_wait(channels[i].sem_w);
    channels[i].buf->finished = 1;
    sem_post(channels[i].sem_r);
  }

  for (size_t i = 0; i != CHILDREN_COUNT; ++i) {
    wait(NULL);
  }

  for (size_t i = 0; i != CHILDREN_COUNT; ++i) {
    munmap(channels[i].buf, sizeof(shm_buffer_t));
    close(channels[i].shm_fd);

    sem_close(channels[i].sem_w);
    sem_close(channels[i].sem_r);

    shm_unlink(channels[i].shm_name);
    sem_unlink(channels[i].sem_w_name);
    sem_unlink(channels[i].sem_r_name);
  }

  return 0;
}