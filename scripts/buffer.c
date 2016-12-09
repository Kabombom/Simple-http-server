#include "../includes/buffer.h"

// Create Request Buffer
void create_buffer() {
  requests_buffer = (Buffer *) malloc(sizeof(Buffer));
  requests_buffer -> request = NULL;
  requests_buffer -> current_size = 0;
  printf("Buffer created\n");
}

// Delete Request Buffer
void delete_buffer() {
  Request *tmp;
  Request *aux = requests_buffer -> request;

  while(aux != NULL) {
    tmp = aux;
    aux = aux->next;
    //free(tmp->required_file);
    free(tmp);
  }
  //free(aux->required_file);
  free(aux);
  free(requests_buffer);
}

// Add request to buffer
void add_request_to_buffer(int ready, int conn, char *required_file, long get_request_time, long serve_request_time) {
  // Create Request
  Request *new_request = (Request *) malloc(sizeof(Request));
  new_request->ready = ready;
  new_request->conn = conn;
  new_request->required_file = (char *) malloc(50 *sizeof(char));
  strcpy(new_request->required_file, required_file);
  new_request->get_request_time = get_request_time;
  new_request->serve_request_time = serve_request_time;


  printf("Adding request to buffer: %s, conn: %d\n", new_request -> required_file, new_request-> conn);
  if (requests_buffer->request == NULL) {
    requests_buffer->request = new_request;
    new_request->next = NULL;
    new_request->prev = NULL;
    return;
  }

  printf("PASSOU ALELUIAAAAAAAAAAAAAAAAAA\n");
  Request *aux = requests_buffer->request;
  new_request->next = aux;
  new_request->prev = NULL;
  requests_buffer->request = new_request;
  requests_buffer->current_size++;
  printf("Current size of buffer_ %d\n", requests_buffer->current_size);
}

// FIFO
Request *get_request_by_fifo() {
  if (requests_buffer->request == NULL) {
    return NULL;
  }

  Request *current_node = requests_buffer->request;
  Request *temp;

  if (current_node != NULL && current_node->next == NULL) {
    printf("current_node eq: %s conn: %d\n", requests_buffer->request->required_file, requests_buffer->request->conn);
    temp = (Request*) malloc(sizeof(Request));
    temp->ready = current_node->ready;
    temp->conn = current_node->conn;
    temp->required_file = (char*)malloc(1024*sizeof(char));
    strcpy(temp->required_file, current_node->required_file);
    temp->get_request_time = current_node->get_request_time;
    temp->serve_request_time = current_node->serve_request_time;
    temp->prev = NULL;
    temp->next = NULL;
    free(current_node);
    requests_buffer->request = NULL;
    printf("temp eq: %s conn: %d\n", temp->required_file, temp->conn);
    return temp;
  }

  Request *previous_node = current_node;
  while (current_node->next != NULL ) {
    previous_node = current_node;
    current_node = current_node->next;
  }

  printf("Removing oldest request from buffer: %s, conn: %d\n", current_node->required_file, current_node->conn);
  temp = (Request*) malloc(sizeof(Request));
  temp->ready = current_node->ready;
  temp->conn = current_node->conn;
  temp->required_file = (char*)malloc(1024*sizeof(char));
  strcpy(temp->required_file, current_node->required_file);
  temp->get_request_time = current_node->get_request_time;
  temp->serve_request_time = current_node->serve_request_time;
  temp->prev = NULL;
  temp->next = NULL;
  previous_node -> next = NULL;
  free(current_node);
  return temp;
}

Request *get_request_by_static() {
  if (requests_buffer->request == NULL) {
    return NULL;
  }

  char cgi_exp[10];
  strcpy(cgi_exp, "cgi-bin/");
  Request *current_node = requests_buffer->request;
  Request *previous_request = NULL;
  Request *oldest_static_request = NULL;
  Request *next_request = NULL;

  if (current_node != NULL && current_node->next == NULL) {
    if(strncmp(current_node->required_file, cgi_exp, strlen(cgi_exp))) {
      oldest_static_request = current_node;
      free(current_node);
      requests_buffer->request = NULL;
      return oldest_static_request;
    }
    return NULL;
  }

  Request *previous_node = current_node;
  if(strncmp(current_node->required_file, cgi_exp, strlen(cgi_exp))) {
    oldest_static_request = current_node;
    next_request = current_node->next;
  }
  while (current_node->next != NULL ) {
    previous_node = current_node;
    current_node = current_node->next;

    if(strncmp(current_node->required_file, cgi_exp, strlen(cgi_exp))) {
      previous_request = previous_node;
      oldest_static_request = current_node;
      next_request = current_node->next;
    }
  }

  Request *temp = oldest_static_request;
  if(strncmp(current_node->required_file, cgi_exp, strlen(cgi_exp))) {
    oldest_static_request = current_node;
    previous_node -> next = NULL;
    printf("Removing oldest static request from buffer: %s\n", oldest_static_request->required_file);
    free(current_node);
    return oldest_static_request;
  }
  else if(previous_request == NULL) {
    next_request->prev = NULL;
    requests_buffer->request = next_request;
    printf("Removing oldest static request from buffer: %s\n", oldest_static_request->required_file);
    temp = oldest_static_request;
    free(temp);
    return oldest_static_request;
  }
  else if(oldest_static_request != NULL){
    previous_request->next = oldest_static_request->next;
    next_request->prev = oldest_static_request->prev;
    printf("Removing oldest static request from buffer: %s\n", oldest_static_request->required_file);
    temp = oldest_static_request;
    free(temp);
    return oldest_static_request;
  }
  return NULL;
}

void print_buffer() {
  Request *aux = requests_buffer -> request;
  int counter = 1;
  while (aux != NULL) {
    printf("Request number %d: %s\n", counter, aux -> required_file);
    printf("time %d: %ld\n", counter, aux -> get_request_time);
    aux = aux -> next;
    counter++;
  }
}
