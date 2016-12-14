#include "../includes/statistics.h"

void print_statistics() {
  long long average_static_time = total_static_time;
  long long average_compressed_time = total_compressed_time;
  long long temporary_number_of_static_requests = number_of_static_requests;
  long long temporary_number_of_compressed_requests = number_of_compressed_requests;

  if (number_of_static_requests != 0)
    average_static_time = total_static_time / temporary_number_of_static_requests;
  if (number_of_compressed_requests != 0)
    average_compressed_time = total_compressed_time / temporary_number_of_compressed_requests;

  printf("Número total de pedidos servidos (páginas estáticas): %ld \n"
  "Número total de pedidos servidos (ficheiros comprimidos): %ld \n"
  "Tempo médio para servir um pedido a conteúdo estático não comprimido: %lld microsegundos\n"
  "Tempo médio para servir um pedido a conteúdo estático comprimido: %lld microsegundos\n",
  number_of_static_requests, number_of_compressed_requests, average_static_time, average_compressed_time);
}

void get_request_information(int type_of_request, char *filename, long request_time, long delivery_time) {
  int fd;
  struct stat sb;
  off_t offset = 0;
  off_t pa_offset;
  size_t length = 100;

  char *str = (char*) malloc(sizeof(char) * 1024);

  char *string_type_of_request = (char*) malloc(1024 * sizeof(char));
  if (type_of_request == 1)
    strcpy(string_type_of_request, "static");
  else
    strcpy(string_type_of_request, "compressed");

  sprintf(str, "%s %s %ld %ld\n", string_type_of_request, filename, request_time, delivery_time);

  if (strcmp(string_type_of_request, "static") == 0) {
    number_of_static_requests++;
    total_static_time += delivery_time - request_time;
  }
  else {
    number_of_compressed_requests++;
    total_compressed_time += delivery_time - request_time;
  }

  fd = open("server.log", O_RDWR, O_APPEND);
  if (fd == -1) {
    printf("Error in opening\n");
  }
  if (fstat(fd, &sb) < 0) {
    printf("Error in fstat\n");
  }

  pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1); /* offset for mmap() must be page aligned */
  length = lseek(fd, 0L, SEEK_END);
  lseek(fd, length-1, SEEK_SET);

  write(fd, str, 55);
}


void reset_statistics() {
  number_of_static_requests = 0;
  number_of_compressed_requests = 0;
  total_static_time = 0;
  total_compressed_time = 0;

  FILE *f = fopen("server.log", "w");
  fclose(f);
}


void memory_mapped_file() {
  number_of_static_requests = 0;
  number_of_compressed_requests = 0;
  total_static_time = 0;
  total_compressed_time = 0;
  int fd;
  char *pmap = (char*) malloc(sizeof(char) * SIZE);
  struct stat sb;
  off_t offset = 0;
  off_t pa_offset;
  size_t length = 100;

  fd = open("server.log", O_RDWR, O_APPEND);
  if (fd == -1) {
    printf("Error in opening\n");
  }
  if (fstat(fd, &sb) < 0) {
    printf("Error in fstat\n");
  }

  // Offset for mmap() must be page aligned
  pa_offset = offset & ~(sysconf(_SC_PAGE_SIZE) - 1);
  length = lseek(fd, 0L, SEEK_END);
  lseek(fd, length-1, SEEK_SET);

  length = lseek(fd, 0L, SEEK_END);
  lseek(fd, length-1, SEEK_SET);

  write(fd, " ", 20);

  pmap = mmap(0, length + offset - pa_offset, PROT_READ | PROT_WRITE, MAP_SHARED, fd, pa_offset);
  close(fd);
  reset_statistics();
}
