#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>

// Project header files
#include "config.h"
#include "buffer.h"
#include "statistics.h"

// Produce debug information
#define DEBUG	  	1

// Header of HTTP reply to client
#define	SERVER_STRING 	"Server: simpleserver/0.1.0\r\n"
#define HEADER_1	"HTTP/1.0 200 OK\r\n"
#define HEADER_2	"Content-Type: text/html\r\n\r\n"

#define GET_EXPR	"GET /"
#define CGI_EXPR	"cgi-bin/"
#define SIZE_BUF	1024

// Pipe variables
#define PIPE_NAME "named_pipe"

// Initial functions
int  fireup(int port);
void identify(int socket);
long get_request(int socket);
int  read_line(int socket, int n);
void send_header(int socket);
void send_page(int socket, char *required_file);
void execute_script(int socket, char *required_file);
void not_found(int socket);
void catch_ctrlc(int);
void cannot_execute(int socket);

//Utils functions
int page_or_script();
int threads_are_available();
char *get_compressed_filename(char *file_path);
int compressed_file_is_allowed(char *filename);
void handle_sigusr1(int signal);

//Processes functions
void statistics();
void create_processes();
void terminate_processes();

// Shared memory functions
void create_shared_memory();
void attach_shared_memory();
void delete_shared_memory();

void create_new_threads(config_struct_aux config_aux);
void handle_console_comands(config_struct_aux config_aux);

void *thread_pipe_routine();
void create_pipe_thread();
void start_pipe();
void read_from_pipe();

void create_semaphores();
void delete_semaphores();
void terminate_thread();
void *worker();
void create_workers();
void delete_scheduler_threads();

void terminate(int what_to_delete, int socket_conn_needs_to_be_closed);

long get_current_time_with_ms();

char buf[SIZE_BUF];
char req_buf[SIZE_BUF];
char buf_tmp[SIZE_BUF];
int port, socket_conn, new_conn, buffer_size;

// Processes ids
int parent_pid;
int config_pid;
int statistics_pid;

// Shared memory ids
int shmid;
int shmid_request;

typedef struct statistics_struct {
  int type;
  char file[1024];
  long get_request_time;
  long serve_request_time;
} Statistics_Struct;

Statistics_Struct *last_request;

int *threads_available;

// Semaphores id
sem_t *sem_buffer_empty; //If buffer is empty the value of the sem is 0
sem_t *sem_buffer_full; //If buffer is full the value of the sem is 0
pthread_mutex_t *buffer_mutex;
pthread_mutex_t *last_request_mutex;

pthread_t *thread_pool;
pthread_t pipe_thread;
pthread_t scheduler;

struct timeval tv;
