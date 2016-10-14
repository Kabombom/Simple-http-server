#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

struct {
  pthread_t *threads;
  pthread_mutex_t mutex;
  sem_t threads_limit;
} scheduler_data

pthread_t *thread_pool;

void create_threads();
