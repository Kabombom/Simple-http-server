#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>


#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define READ_SIZE 200
#define PIPE_NAME "named_pipe"

typedef struct {
  int serverport;
  char *scheduling;
  int thread_pool;
  char *allowed;
} config_struct;

typedef struct {
  int a;
  int b;
  int c;
} numbers;


config_struct *config;

void configuration_start();
void start_pipe();
void read_from_pipe();
void write_in_pipe();
