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
void add_request_to_buffer(int ready, char *required_file, time_t get_request_time, time_t serve_request_time) {
  Request *new_request = (Request *) malloc(sizeof(Request));
  new_request->ready = ready;
  new_request->required_file = (char *) malloc(50 *sizeof(char));
  strcpy(new_request->required_file, required_file);
  new_request->get_request_time = get_request_time;
  new_request->serve_request_time = serve_request_time;
  new_request->next = NULL;
  new_request->prev = NULL;

  printf("Adding request to buffer: %s\n", new_request -> required_file);
  printf("2Current size of buffer_ %d\n", requests_buffer->current_size);
  if (requests_buffer->request == NULL) {
    printf("ESTA MERDA\n");
    requests_buffer->request = new_request;
    new_request->next = NULL;
    return;
  }

  Request *aux = requests_buffer->request;
  while(aux -> next != NULL) {
    aux = aux->next;
  }

  aux->next = new_request;
  new_request->prev = aux;
  new_request->next = NULL;
  requests_buffer->current_size++;
  printf("Current size of buffer_ %d\n", requests_buffer->current_size);
}

// Remove requestr from buffer - FIFO
Request *remove_request_from_buffer() {
  if (requests_buffer->request == NULL) {
    return NULL;
  }

  Request *currentNode = requests_buffer->request;
  Request *temp;

  if (currentNode != NULL && currentNode->next == NULL) {
    temp = currentNode;
    free(currentNode);
    requests_buffer->request = NULL;
    return temp;
  }

  Request *previousNode = currentNode;
  while (currentNode->next != NULL ) {
    previousNode = currentNode;
    currentNode = currentNode->next;
  }

  printf("Removing oldest request from buffer: %s\n", currentNode->required_file);
  temp = currentNode;
  previousNode -> next = NULL;
  free(currentNode);
  return temp;
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

