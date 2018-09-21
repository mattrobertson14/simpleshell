CC= gcc
CFLAGS= -Wall -g
SRCS= simpleshell.c
OBJS= simpleshell.o
PROGRAM= simpleshell
MKDEP= gccmakedep

${PROGRAM}:	${OBJS}
	${CC} ${CFLAGS} ${SRCS} -o ${PROGRAM}

%.o : %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f ${OBJS} ${PROGRAM}

depend:
	${MKDEP} ${CFLAGS} ${SRCS}
