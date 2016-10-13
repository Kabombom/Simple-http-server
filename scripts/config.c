#include "../includes/config.h"

void configuration_start() {
  FILE *configuration_file = fopen("../data/config.txt")
  char *serverport = (char *) malloc(SIZE_BUF * sizeof(char));
  char *scheduling = (char *) malloc(SIZE_BUF * sizeof(char));
  char *threadpool = (char *) malloc(SIZE_BUF * sizeof(char));
  char *allowed = (char *) malloc(SIZE_BUF * sizeof(char));

  fscanf(configuration_file, "%[^\n]", serverport);
  fseek(configuration_file, 1, SEEK_CUR);
  fscanf(configuration_file, "%[^\n]", scheduling);
  fseek(configuration_file, 1, SEEK_CUR);
  fscanf(configuration_file, "%[^\n]", threadpool);
  fseek(configuration_file, 1, SEEK_CUR);
  fscanf(configuration_file, "%[^\n]", allowed);
  printf("%s\n", serverport);
  printf("%s\n", scheduling);
  printf("%s\n", threadpool);
  printf("%s\n", allowed);
  fclose(configuration_file);
  return;
}

