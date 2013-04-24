include ../Make.defines.macos
EXTRA=-DDEBUG

PROGS = call
HDRS = call.h

all:	${PROGS} 

call.o:		call.c ${HDRS}

main.o:		main.c ${HDRS}

put.o:		put.c ${HDRS}

take.o:		take.c ${HDRS}

takeput.o:		takeput.c ${HDRS}

escape.o:	escape.c ${HDRS}

loop.poll.o:	loop.poll.c ${HDRS}

call:		call.o main.o put.o take.o takeput.o escape.o loop.poll.o
		$(CC) $(CFLAGS) -o call main.o call.o put.o take.o takeput.o escape.o loop.poll.o ../sockets/clconn.o $(LDFLAGS) $(LDLIBS)

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o

