#include "../includes/buffer.h"

//1 for script, 0 for page
int is_script(char *filename) {
  char cgi_exp[10];
  strcpy(cgi_exp, "cgi-bin/");

  if(!strncmp(filename, cgi_exp, strlen(cgi_exp))) {
    return 1;
  }
  return 0;
}


// Create Request Buffer
void create_buffer() {
  requests_buffer = (Buffer *) malloc(sizeof(Buffer));
  requests_buffer -> request = NULL;
  requests_buffer -> current_size = 0;
  printf("Buffer created\n");
}

// Delete Request Buffer
void delete_buffer() {
  if (requests_buffer->request == NULL) {
    return;
  }
  Request *tmp;
  Request *aux = requests_buffer -> request;

  while(aux->next != NULL) {
    tmp = aux;
    aux = aux->next;
    free(tmp);
  }
  free(aux);
  requests_buffer->request = NULL;
  requests_buffer->current_size = 0;
}

// Add request to buffer
void add_request_to_buffer(int type, int conn, char *required_file, long get_request_time, long serve_request_time) {
  // Create Request
  Request *new_request = (Request *) malloc(sizeof(Request));
  new_request->type = type;
  new_request->conn = conn;
  new_request->required_file = (char *) malloc(REQUIRED_FILE_SIZE *sizeof(char));
  strcpy(new_request->required_file, required_file);
  new_request->get_request_time = get_request_time;
  new_request->serve_request_time = serve_request_time;


  printf("Adding request to buffer: %s, conn: %d\n", new_request -> required_file, new_request-> conn);
  if (requests_buffer->request == NULL) {
    requests_buffer->request = new_request;
    new_request->next = NULL;
    requests_buffer->current_size++;
    printf("Current size of buffer_ %d\n", requests_buffer->current_size);
    return;
  }

  Request *aux = requests_buffer->request;
  new_request->next = aux;
  requests_buffer->request = new_request;
  requests_buffer->current_size++;
  printf("Current size of buffer_ %d\n", requests_buffer->current_size);
}

// Add request according to static priority
void add_static_request_to_buffer(int type, int conn, char *required_file, long get_request_time, long serve_request_time) {
  int new_request_is_script = is_script(required_file);
  if(new_request_is_script == 1) {
    add_request_to_buffer(type, conn, required_file, get_request_time, serve_request_time);
    return;
  }

  // Create Request
  Request *current_node = requests_buffer->request;
  Request *new_request = (Request *) malloc(sizeof(Request));
  new_request->type = type;
  new_request->conn = conn;
  new_request->required_file = (char *) malloc(REQUIRED_FILE_SIZE *sizeof(char));
  strcpy(new_request->required_file, required_file);
  new_request->get_request_time = get_request_time;
  new_request->serve_request_time = serve_request_time;

  printf("Adding request to buffer: %s, conn: %d\n", new_request -> required_file, new_request-> conn);
  if(current_node == NULL) {
    requests_buffer->request = new_request;
    new_request->next = NULL;
    requests_buffer->current_size++;
    return;
  }
  else if(current_node != NULL && current_node->next == NULL) {
    if (is_script(current_node->required_file) == 0) {
      new_request->next = current_node;
      requests_buffer->request = new_request;
      requests_buffer->current_size++;
      return;
    }
    else {
      current_node->next = new_request;
      new_request->next = NULL;
      requests_buffer->current_size++;
      return;
    }
  }

  while(current_node->next != NULL && is_script(current_node->next->required_file) != 0) {
    current_node = current_node->next;
  }
  if (current_node->next == NULL) {
    current_node->next = new_request;
    new_request->next = NULL;
    requests_buffer->current_size++;
    return;
  }
  new_request->next = current_node->next;
  current_node->next = new_request;
  requests_buffer->current_size++;
  return;
}

// Add request according to static priority
void add_compressed_request_to_buffer(int type, int conn, char *required_file, long get_request_time, long serve_request_time) {
  int new_request_is_script = is_script(required_file);
  if(new_request_is_script == 0) {
    add_request_to_buffer(type, conn, required_file, get_request_time, serve_request_time);
    return;
  }

  // Create Request
  Request *current_node = requests_buffer->request;
  Request *new_request = (Request *) malloc(sizeof(Request));
  new_request->type = type;
  new_request->conn = conn;
  new_request->required_file = (char *) malloc(REQUIRED_FILE_SIZE *sizeof(char));
  strcpy(new_request->required_file, required_file);
  new_request->get_request_time = get_request_time;
  new_request->serve_request_time = serve_request_time;

  printf("Adding request to buffer: %s, conn: %d\n", new_request -> required_file, new_request-> conn);
  if(current_node == NULL) {
    requests_buffer->request = new_request;
    new_request->next = NULL;
    return;
  }
  else if(current_node != NULL && current_node->next == NULL) {
    if (is_script(current_node->required_file) == 1) {
      new_request->next = current_node;
      requests_buffer->request = new_request;
      requests_buffer->current_size++;
      return;
    }
    else {
      current_node->next = new_request;
      new_request->next = NULL;
      requests_buffer->current_size++;
      return;
    }
  }

  while(current_node->next != NULL && is_script(current_node->next->required_file) != 1) {
    current_node = current_node->next;
  }
  if (current_node->next == NULL) {
    current_node->next = new_request;
    new_request->next = NULL;
    requests_buffer->current_size++;
    return;
  }
  new_request->next = current_node->next;
  current_node->next = new_request;
  requests_buffer->current_size++;
  return;
}

// FIFO
Request *get_request_by_fifo() {
  if (requests_buffer->request == NULL) {
    return NULL;
  }

  Request *previous_node;
  Request *current_node = requests_buffer->request;
  Request *temp;

  if (current_node != NULL && current_node->next == NULL) {
    temp = (Request*) malloc(sizeof(Request));
    temp->type = current_node->type;
    temp->conn = current_node->conn;
    temp->required_file = (char *) malloc(1024 * sizeof(char));
    strcpy(temp->required_file, current_node->required_file);
    temp->get_request_time = current_node->get_request_time;
    temp->serve_request_time = current_node->serve_request_time;
    temp->next = NULL;
    free(current_node);
    requests_buffer->request = NULL;
    requests_buffer->current_size--;
    return temp;
  }

  while (current_node->next != NULL ) {
    previous_node = current_node;
    current_node = current_node->next;
  }

  printf("Removing oldest request from buffer: %s, conn: %d\n", current_node->required_file, current_node->conn);
  temp = (Request*) malloc(sizeof(Request));
  temp->type = current_node->type;
  temp->conn = current_node->conn;
  temp->required_file = (char*)malloc(1024*sizeof(char));
  strcpy(temp->required_file, current_node->required_file);
  temp->get_request_time = current_node->get_request_time;
  temp->serve_request_time = current_node->serve_request_time;
  temp->next = NULL;
  previous_node->next = NULL;
  requests_buffer->current_size--;
  free(current_node);
  return temp;
}

void print_buffer() {
  if (requests_buffer->request == NULL) {
    printf("Buffer is empty\n");
    return;
  }
  Request *aux = requests_buffer -> request;
  int counter = 1;
  while (aux != NULL) {
    printf("Request number %d: %s\n", counter, aux -> required_file);
    printf("time %d: %ld\n", counter, aux -> get_request_time);
    aux = aux -> next;
    counter++;
  }
}

/* function to swap data of two nodes a and b*/
void swap(Request *a, Request *b) {
  int type = a->type;
  int conn = a->conn;
  char *required_file = a->required_file;
  time_t get_request_time = a->get_request_time;
  time_t serve_request_time = a->serve_request_time;
  a->type = b->type;
  a->conn = b->conn;
  strcpy(a->required_file, b->required_file);
  //a->required_file = b->required_file;
  a->get_request_time = b->get_request_time;
  a->serve_request_time = b->serve_request_time;
  b->type = type;
  b->conn = conn;
  //b->required_file = required_file;
  strcpy(b->required_file, required_file);
  b->get_request_time = get_request_time;
  b->serve_request_time = serve_request_time;
}

// bubbleSort to order linked list based on the time the request was received and
// based on which type of file it is
// 0 for static pages
// 1 for compressed files
// 3 for regular sort
void bubbleSort(int script) {
  int swapped;
  Request *ptr1;
  Request *lptr = NULL;

  /* Checking for empty list */
  if (requests_buffer->request == NULL || requests_buffer->request->next == NULL)
    return;

  if (script == 0) {
    do {
      swapped = 0;
      ptr1 = requests_buffer->request;

      while (ptr1->next != lptr) {
        int ptr1_is_script = is_script(ptr1->required_file);
        int ptr1_next_is_script = is_script(ptr1->next->required_file);

        if (ptr1_is_script == 0 && ptr1_next_is_script == 1) {
          swap(ptr1, ptr1->next);
          swapped = 1;
        }
        else if (ptr1_is_script == 1 && ptr1_next_is_script == 1) {
          if (ptr1->get_request_time > ptr1->next->get_request_time) {
            swap(ptr1, ptr1->next);
            swapped = 1;
          }
        }
        else if (ptr1_is_script == 0) {
          if (ptr1->get_request_time > ptr1->next->get_request_time) {
            swap(ptr1, ptr1->next);
            swapped = 1;
          }
        }
        ptr1 = ptr1->next;
      }
      lptr = ptr1;
    } while (swapped);
    return;
  }
  if (script == 1) {
    do {
      swapped = 0;
      ptr1 = requests_buffer->request;

      while (ptr1->next != lptr) {
        int ptr1_is_script = is_script(ptr1->required_file);
        int ptr1_next_is_script = is_script(ptr1->next->required_file);

        if (ptr1_is_script == 1 && ptr1_next_is_script == 0) {
          swap(ptr1, ptr1->next);
          swapped = 1;
        }
        else if (ptr1_is_script == 0 && ptr1_next_is_script == 0) {
          if (ptr1->get_request_time > ptr1->next->get_request_time) {
            swap(ptr1, ptr1->next);
            swapped = 1;
          }
        }
        else if (ptr1_is_script == 1) {
          if (ptr1->get_request_time > ptr1->next->get_request_time) {
            swap(ptr1, ptr1->next);
            swapped = 1;
          }
        }
        ptr1 = ptr1->next;
      }
      lptr = ptr1;
    } while (swapped);
    return;
  }
  do {
    swapped = 0;
    ptr1 = requests_buffer->request;

    while (ptr1->next != lptr) {
      if (ptr1->get_request_time > ptr1->next->get_request_time) {
        swap(ptr1, ptr1->next);
        swapped = 1;
      }
      ptr1 = ptr1->next;
    }
    lptr = ptr1;
  } while (swapped);
  return;
}
