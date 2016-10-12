CC = gcc
CFLAGS = -Wall -g

simplehttpd: simplehttpd.o header.h
	$(CC) $(CFLAGS) -o simplehttpd simplehttpd.o
simplehttpd.o: simplehttpd.c header.h
	$(CC) $(CFLAGS) -c simplehttpd.c
