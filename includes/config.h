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

typedef struct {
  int serverport;
  char *scheduling;
  int thread_pool;
  char *allowed;
} config_struct;

typedef struct {
  int option;
  char change[50];
} config_struct_aux;

config_struct *config;

void configuration_start();
void change_configuration_file();
