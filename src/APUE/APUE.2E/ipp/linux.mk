include ../Make.defines.linux
EXTRA=-DDEBUG

PROGS = print printd
HDRS = print.h ipp.h

all:	${PROGS} 

util.o:		util.c ${HDRS}

print.o:	print.c ${HDRS}

printd.o:	printd.c ${HDRS}

print:		print.o util.o
		$(CC) $(CFLAGS) -o print print.o util.o ../sockets/clconn.o $(LDFLAGS) $(LDLIBS)

printd:		printd.o util.o
		$(CC) $(CFLAGS) -o printd printd.o util.o ../sockets/clconn.o ../sockets/initsrv2.o $(LDFLAGS) $(LDLIBS) -pthread

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o

