#include "../includes/buffer.h"

// Create Request Buffer
void create_buffer() {
  requests_buffer = (Buffer *) malloc(sizeof(Buffer));
  requests_buffer -> request = NULL;
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
