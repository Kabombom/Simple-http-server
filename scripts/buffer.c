#include "../includes/buffer.h"

// Create Request Buffer
void create_buffer() {
  requests_buffer = (Buffer *) malloc(sizeof(Buffer));
  requests_buffer -> request = NULL;
}

// Delete Request Buffer
void delete_buffer() {
  Request *tmp;

  while(requests_buffer->next != NULL) {
    tmp = requests_buffer->request;
    requests_buffer->request = requests_buffer->request->next;
    free(tmp->required_files);
    free(tmp);
  }

  free(requests_buffer->request->required_files);
  free(requests_buffer->request);
  free(requests_buffer);
}

void add_request_to_buffer(Request *new_request) {
  if (requests_buffer->request == NULL) {
    requests_buffer->request = new_request;
    return;
  }

  Request *aux = requests_buffer->request;
  while(aux -> next != NULL)
    aux = aux -> next;

  aux -> next = new_request;
  new_request -> previous = aux;
}
