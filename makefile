CC = gcc
CFLAGS = -pthread -Wall -g
OBJS = scripts/simplehttpd.o scripts/config.o scripts/scheduler.o
PROG = simplehttpd
PATH_simplehttpd.c = scripts/simplehttpd.c
PATH_header.h = includes/header.h
PATH_config.c = scripts/config.c
PATH_config.h = config.h
PATH_scheduler.c = scripts/scheduler.c
PATH_scheduler.h = scripts/scheduler.h

all:			${PROG}
clean:		rm ${OBJS} *~ ${PROG}
${PROG}:	${OBJS}
					${CC} ${CFLAGS} ${OBJS} -o $@
.c.o:			${CC} ${CFLAGS} $< -c -o $@

simplehttpd.o: PATH_simplehttpd.c PATH_header.h
config.o:  PATH_config.h
scheduler.o:  PATH_scheduler.h
