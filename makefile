CC = gcc
CFLAGS = -Wall -g

simplehttpd: scripts/simplehttpd.o scripts/config.o 
	$(CC) $(CFLAGS) -o simplehttpd scripts/simplehttpd.o scripts/config.o

simplehttpd.o: scripts/simplehttpd.c includes/header.h
	$(CC) $(CFLAGS) -c scripts/simplehttpd.c

config.o: scripts/config.c includes/config.h
	$(CC) $(CFLAGS) -c scripts/config.c
