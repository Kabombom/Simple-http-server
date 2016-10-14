#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>

#define READ_SIZE 200

typedef struct {
  int serverport;
  char *scheduling;
  int thread_pool;
  char *allowed;
} config_struct;

config_struct *config;

void configuration_start();
