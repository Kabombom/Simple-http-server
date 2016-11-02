#include "../includes/scheduler.h"
#include "../includes/config.h"

// Threads routine
void *scheduler_thread_routine() {
  printf("This is a thread.\n");
  return NULL;
}

// Create pool of threads
void create_scheduler_threads() {
  int i;
  long ids[config -> thread_pool];
  // Create pool of threads
  thread_pool = malloc(sizeof(pthread_t) * config->thread_pool);

  if (thread_pool == NULL) {
    perror("Error allocating memory for threads\n");
  }

  // Create threads
  for (i = 0; i < config->thread_pool; i++) {
    ids[i] = i;
    if (pthread_create(&thread_pool[i], NULL, scheduler_thread_routine, (void *)ids[i]) != 0) {
      perror("Error creating thread");
    }
  }

  // Wait for all threads to complete
  for (i = 0; i < config->thread_pool; i++) {
    pthread_join(thread_pool[i], NULL);
  }
}

// Delete threads
void delete_scheduler_threads() {
  printf("Cleaning up scheduler...\n");
  int i;
  for (i = 0; i < config->thread_pool; i++) {
    pthread_join(thread_pool[i], NULL);
  }
}
