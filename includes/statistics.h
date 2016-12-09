#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h> // Open function
#include <unistd.h> // Close function
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#define SIZE 10000
char *src;
int fd;

void print_statistics();
void get_request_information(int type_of_request, char *file, long request_time, long delivery_time);
