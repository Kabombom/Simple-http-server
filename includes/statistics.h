#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h> // Open function
#include <unistd.h> // Close function
#include <sys/stat.h>

void print_statistics();
void get_request_information(char *type_of_request, char *file, time_t request_time, time_t delivery_time);
