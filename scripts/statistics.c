#include "../includes/statistics.h"

void print_statistics() {

  char file[SIZE] = "mmf.txt";

  if ((fd = open(file, O_RDWR | O_TRUNC, FILESEC_MODE)) < 0) {
    fprintf(stderr, "can't creat %s for writing\n", file);
    exit(1);
  }

  if ((src = mmap ((caddr_t)0, SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0)) == (caddr_t) -1) {
    printf("mmap error for input\n");
    exit(1);
  }

  printf("Source %s\n", src );
  printf("Número total de pedidos servidos (páginas estáticas): \n"
  "Número total de pedidos servidos (ficheiros comprimidos): \n"
  "Tempo médio para servir um pedido a conteúdo estático não comprimido: \n"
  "Tempo médio para servir um pedido a conteúdo estático comprimido: \n");
}

void get_request_information(char *type_of_request, char *filename, time_t request_time, time_t delivery_time) {
  char file[SIZE] = "mmf.txt";
  char *src = (char *) malloc(SIZE * sizeof(char));
  char str[100];

  sprintf(str, "%s %s %ld %ld\n", type_of_request, filename, request_time, delivery_time);

	if ((fd = open(file, O_RDWR | O_CREAT | O_TRUNC, FILESEC_MODE)) < 0) {
		fprintf(stderr, "can't creat %s for writing\n", file);
		exit(1);
	}

  if ((src = mmap ((caddr_t)0, SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 0)) == (caddr_t) -1) {
    printf("mmap error for input\n");
    exit(1);
  }

  write(fd, str, 100);
  printf("Src %s\n", src );
}
