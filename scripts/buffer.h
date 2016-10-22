#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int ready;
  int conn;
  char *required_file;
  time_t get_request_time;
	time_t serve_request_time;
} node;

typedef struct {
  node request;
  struct buffer *next;
} buffer;
