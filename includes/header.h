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
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <fcntl.h>

// Project header files
#include "semlib.h"
#include "config.h"
#include "scheduler.h"
#include "buffer.h"

// Produce debug information
#define DEBUG	  	1

// Header of HTTP reply to client
#define	SERVER_STRING 	"Server: simpleserver/0.1.0\r\n"
#define HEADER_1	"HTTP/1.0 200 OK\r\n"
#define HEADER_2	"Content-Type: text/html\r\n\r\n"

#define GET_EXPR	"GET /"
#define CGI_EXPR	"cgi-bin/"
#define SIZE_BUF	1024
#define BUFFER_SIZE 10 // Maximum number of requests in buffer

// Pipe variables
#define PIPE_NAME "named_pipe"

// Initial functions
int  fireup(int port);
void identify(int socket);
void get_request(int socket);
int  read_line(int socket, int n);
void send_header(int socket);
void send_page(int socket);
void execute_script(int socket);
void not_found(int socket);
void catch_ctrlc(int);
void cannot_execute(int socket);

//Processes functions
void statistics();
void create_processes();
void terminate_processes();

// Shared memory functions
void create_shared_memory();
void attach_shared_memory();
void delete_shared_memory();

void *thread_pipe_routine();
void create_pipe_thread();
void start_pipe();
void read_from_pipe();

char buf[SIZE_BUF];
char req_buf[SIZE_BUF];
char buf_tmp[SIZE_BUF];
int port, socket_conn, new_conn;

// Processes ids
int parent_pid;
int config_pid;
int statistics_pid;

// Shared memory ids
int shmid;

// Semaphores id
int semid;

pthread_t pipe_thread;
