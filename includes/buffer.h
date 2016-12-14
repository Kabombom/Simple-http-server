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

#define REQUIRED_FILE_SIZE 500

typedef struct request {
  int type;
  int conn;
  char *required_file;
  long get_request_time;
	long serve_request_time;
  struct request *prev;
  struct request *next;
} Request;

typedef struct buffer {
  int current_size;
  Request *request;
} Buffer;

Buffer *requests_buffer;

int is_script(char *filename);
void create_buffer();
void delete_buffer();
void add_request_to_buffer(int type, int conn, char *required_file, long get_request_time, long serve_request_time);
void add_static_request_to_buffer(int type, int conn, char *required_file, long get_request_time, long serve_request_time);
void add_compressed_request_to_buffer(int type, int conn, char *required_file, long get_request_time, long serve_request_time);
Request *get_request_by_fifo();
void print_buffer();
void swap(Request *a, Request *b);
void bubbleSort(int script);
