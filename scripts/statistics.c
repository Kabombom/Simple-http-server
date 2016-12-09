#include "../includes/statistics.h"

void print_statistics() {
  long number_of_static_requests = 0;
  long number_of_compressed_requests = 0;
  long average_static_time = 0;
  long average_compressed_time = 0;

  char str[] = "static index.html 1481306872087 1481306872088\nstatic index.html 1481306872090 1481306872100\ncompressed index.html 1481306872087 1481306872090\ncompressed index.html 1481306872090 1481306872100";
  char *end_str;
  char *token = strtok_r(str, "\n", &end_str);
  int index;

  while (token != NULL) {
    // printf("%s\n", token );
    index = 0;
    int is_static = 0;
    long beg_time;
    char *end_token;
    char *token2 = strtok_r(token, " ", &end_token);
    while (token2 != NULL) {
      // printf("%s\n", token2 );
      if (index == 0) {
        if (strcmp(token2, "static") == 0) {
          number_of_static_requests++;
          is_static = 1;
        }
        else
          number_of_compressed_requests++;
      }
      else if (index == 2) {
        beg_time = atol(token2);
      }
      else if (index == 3) {
        if (is_static) {
          average_static_time += atol(token2) - beg_time;
        }
        else
          average_compressed_time += atol(token2) - beg_time;
      }
      index++;
      token2 = strtok_r(NULL, " ", &end_token);
    }
    token = strtok_r(NULL, "\n", &end_str);
  }
  if (number_of_static_requests != 0)
    average_static_time = average_static_time / number_of_static_requests;
  if (number_of_compressed_requests != 0)
    average_compressed_time = average_compressed_time / number_of_compressed_requests;

  printf("Número total de pedidos servidos (páginas estáticas): %ld \n"
  "Número total de pedidos servidos (ficheiros comprimidos): %ld \n"
  "Tempo médio para servir um pedido a conteúdo estático não comprimido: %ld ms\n"
  "Tempo médio para servir um pedido a conteúdo estático comprimido: %ld ms\n",
  number_of_static_requests, number_of_compressed_requests, average_static_time, average_compressed_time);
}

void get_request_information(int type_of_request, char *filename, long request_time, long delivery_time) {
  char file[SIZE] = "mmf.log";
  src = (char *) malloc(SIZE * sizeof(char));
  char str[100];

  char *string_type_of_request = (char*) malloc(1024 * sizeof(char));
  if (type_of_request == 1)
    strcpy(string_type_of_request, "static");
  else
    strcpy(string_type_of_request, "Compressed");

  sprintf(str, "%s %s %ld %ld\n", string_type_of_request, filename, request_time, delivery_time);

	if ((fd = open(file, O_RDWR)) < 0) {
		printf("ERROR IN OPEN\n");
		exit(1);
	}

  if ((src = mmap ((caddr_t)0, SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, fd, 0)) == (caddr_t) -1) {
    printf("ERROR IN NMAP\n");
    exit(1);
  }

  write(fd, str, 100);
  printf("Src %s\n", src );
}
