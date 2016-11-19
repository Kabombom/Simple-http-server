#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "config.h"

#define PIPE_NAME "named_pipe"
#define MAX_CHAR 32

void menu();
int menu_protection();
void write_in_pipe(config_struct_aux config);
