#include "../includes/buffer.h"

// Create Request Buffer
void create_buffer() {
  requests_buffer = (Buffer *) malloc(sizeof(Buffer));
  requests_buffer -> request = NULL;
  printf("Buffer created\n");
}

// Delete Request Buffer
void delete_buffer() {
  Request *tmp;
  Request *aux = requests_buffer->request;

  while(aux != NULL) {
    tmp = aux;
    aux = aux->next;
    free(tmp->required_file);
    free(tmp);
  }

  free(aux->required_file);
  free(aux);
  free(requests_buffer);
}

// Add request to buffer
void add_request_to_buffer(Request *new_request) {
  printf("Adding a request to buffer\n");
  if (requests_buffer->request == NULL) {
    requests_buffer->request = new_request;
    return;
  }
  Request *aux = requests_buffer->request;
  while(aux -> next != NULL)
    aux = aux -> next;

  aux -> next = new_request;
  new_request -> prev = aux;
}

void print_buffer() {
  Request *aux = requests_buffer -> request;
  int counter = 1;
  while (aux != NULL) {
    printf("Request number %d: %s\n", counter, aux -> required_file);
    aux = aux -> next;
    counter++;
  }
}
