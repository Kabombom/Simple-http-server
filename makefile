CC = gcc
CFLAGS = -Wall -g

simplehttpd: scripts/simplehttpd.o includes/header.h includes/config.h
	$(CC) $(CFLAGS) -o simplehttpd scripts/simplehttpd.o
simplehttpd.o: scripts/simplehttpd.c includes/header.h includes/config.h
	$(CC) $(CFLAGS) -c scripts/simplehttpd.c
