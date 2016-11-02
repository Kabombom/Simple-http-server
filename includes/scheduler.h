#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

typedef struct {
  pthread_t *threads;
  pthread_mutex_t mutex;
} scheduler_data;

pthread_t *thread_pool;

void delete_scheduler_threads();
void *scheduler_thread_routine();
void create_scheduler_threads();
