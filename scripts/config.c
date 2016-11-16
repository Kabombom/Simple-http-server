#include "../includes/config.h"

void configuration_start() {
  // Allocate memory
  config = (config_struct *) malloc(sizeof(config_struct));
  char *serverport = (char *) malloc(READ_SIZE * sizeof(char));
  char *scheduling = (char *) malloc(READ_SIZE * sizeof(char));
  char *threadpool = (char *) malloc(READ_SIZE * sizeof(char));
  char *allowed = (char *) malloc(READ_SIZE * sizeof(char));

  // Read values from file
  FILE *configuration_file = fopen("./data/config.txt", "r");
  fscanf(configuration_file, "%[^\n]", serverport);
  fseek(configuration_file, 1, SEEK_CUR);
  fscanf(configuration_file, "%[^\n]", scheduling);
  fseek(configuration_file, 1, SEEK_CUR);
  fscanf(configuration_file, "%[^\n]", threadpool);
  fseek(configuration_file, 1, SEEK_CUR);
  fscanf(configuration_file, "%[^\n]", allowed);
  fclose(configuration_file);

  // Place those values on the config_struct
  config -> serverport = atoi(serverport);
  config -> scheduling = scheduling;
  config -> thread_pool = atoi(threadpool);
  config -> allowed = allowed;
  return;
}

void start_pipe() {
  // Creates the named pipe if it doesn't exist yet
  if (mkfifo(PIPE_NAME, O_CREAT|O_EXCL|0600)<0 && (errno != EEXIST)) {
    perror("Cannot create pipe: ");
    exit(0);
  }
  printf("Named pipe created.\n");
}

void read_from_pipe() {
  // Opens the pipe for reading
  int fd;
  if ((fd = open(PIPE_NAME, O_RDONLY)) < 0) {
      perror("Cannot open pipe for reading: ");
      exit(0);
  }

  // Do some work
  numbers n;
  while (1) {
    read(fd, &n, sizeof(numbers));
    printf("[SERVER] Received (%d,%d), adding it: %d\n", n.a, n.b, n.a+n.b);
  }
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
    numbers n;
    n.a = rand() % 100;
    n.b = rand() % 100;
    printf("[CLIENT] Sending (%d,%d) for adding\n", n.a, n.b);
    write(fd, &n, sizeof(numbers));
    sleep(2);
  }
}
