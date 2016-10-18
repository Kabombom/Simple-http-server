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

// Project header files
#include "config.h"
#include "scheduler.h"

// Produce debug information
#define DEBUG	  	1	

// Header of HTTP reply to client 
#define	SERVER_STRING 	"Server: simpleserver/0.1.0\r\n"
#define HEADER_1	"HTTP/1.0 200 OK\r\n"
#define HEADER_2	"Content-Type: text/html\r\n\r\n"

#define GET_EXPR	"GET /"
#define CGI_EXPR	"cgi-bin/"
#define SIZE_BUF	1024


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
void create_shared_memory();
void attach_shared_memory();
void delete_shared_memory();

char buf[SIZE_BUF];
char req_buf[SIZE_BUF];
char buf_tmp[SIZE_BUF];
int port, socket_conn, new_conn;

int semid;
int shmid;
int parent_pid;
int config_pid;
int statistics_pid;

