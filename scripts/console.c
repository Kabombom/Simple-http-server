#include "../includes/console.h"

void menu() {
  int choice;
}

void write_in_pipe() {
  // Opens the pipe for writing
  int fd;
  if ((fd = open(PIPE_NAME, O_WRONLY)) < 0) {
    perror("Cannot open pipe for writing: ");
    exit(0);
  }
  
  // Do some work
  while (1) {
    config_struct new_config;
    printf("[CLIENT] Sending (%d,%d) for adding\n", n.a, n.b);
    write(fd, &n, sizeof(numbers));
    sleep(2);
  }
}
