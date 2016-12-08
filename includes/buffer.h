#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>

typedef struct request {
  int ready;
  int conn;
  char *required_file;
  time_t get_request_time;
	time_t serve_request_time;
  struct request *prev;
  struct request *next;
} Request;

typedef struct buffer {
  int current_size;
  Request *request;
} Buffer;

Buffer *requests_buffer;

void create_buffer();
void delete_buffer();
void add_request_to_buffer(int ready, int conn, char *required_file, time_t get_request_time, time_t serve_request_time);
Request *get_request_by_fifo();
Request *get_request_by_static();
Request *get_request_by_compressed();
void print_buffer();
