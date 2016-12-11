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

#define SIZE 1024

char *pmap;

long number_of_static_requests;
long number_of_compressed_requests;
long long total_static_time;
long long total_compressed_time;

void memory_mapped_file();
void print_statistics();
void get_request_information(int type_of_request, char *file, long request_time, long delivery_time);
void reset_statistics();
