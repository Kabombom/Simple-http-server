#include "../includes/scheduler.h"
/*
// Create semaphores
void create_semaphores(int number_of_sems) {
  semid = sem_get(number_of_sems, 1); // Creates a new array of semaphores with init_val = 1
  if (semid == -1) {
    perror("Failed to create semaphores");
    exit(1);
  }
}

// Delete semaphores
void delete_semaphores() {
  printf("Deleting semaphores...\n");
  sem_close(semid);
}

void terminate_thread() {
  printf("Terminating thread...\n");
  pthread_exit(0);
}

// Threads routine
void *scheduler_thread_routine() {
  while(1) {
    sem_wait(semid, 0);
    pthread_mutex_lock(&mutex);
    if (strcmp(config->scheduling, "NORMAL") == 0) {
      Request *request = remove_request_from_buffer();
      if (request != NULL) {
        printf("request removed: %s\n", request->required_file);
      }
    }
    pthread_mutex_unlock(&mutex);
    sem_post(semid, 0);
    sleep(1);
  }
  return NULL;
}

// Create pool of threads
void create_scheduler_threads() {
  signal(SIGUSR1, terminate_thread);
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
}

// Delete threads
void delete_scheduler_threads() {
  printf("Cleaning up scheduler...\n");
  int i;
  for (i = 0; i < config->thread_pool; i++) {
    pthread_join(thread_pool[i], NULL);
  }
}
*/
