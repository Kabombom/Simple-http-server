#include "../includes/statistics.h"

void print_statistics() {
  printf("Número total de pedidos servidos (páginas estáticas): \n"
  "Número total de pedidos servidos (ficheiros comprimidos): \n"
  "Tempo médio para servir um pedido a conteúdo estático não comprimido: \n"
  "Tempo médio para servir um pedido a conteúdo estático comprimido: \n");
}

void get_request_information(char *type_of_request, char *file, time_t request_time, time_t delivery_time) {
  int fdout = open("../data/mmf.txt", O_RDWR | O_CREAT | O_TRUNC, FILESEC_MODE);
  char *addr = "text";
  off_t offset, pa_offset;
  size_t length = 100;
  offset = 1;
  ssize_t s;
  pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);

  printf("%s\n", type_of_request );
  printf("%s\n", file );
  printf("%ld\n", request_time);
  printf("%ld\n", delivery_time);

  // Testing Writing on MMF
  s = write(fdout, addr, length);
}
