#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "../includes/buffer.h"
#include "../includes/config.h"
#include "../includes/semlib.h"

typedef struct {
  pthread_t *threads;
  pthread_mutex_t mutex;
} scheduler_data;

pthread_t *thread_pool;

void delete_scheduler_threads();
void terminate_thread();
void *scheduler_thread_routine();
void create_scheduler_threads();

void create_semaphores(int number_of_sems);
void delete_semaphores();

// Semaphores id
int semid;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
