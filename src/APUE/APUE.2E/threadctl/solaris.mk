include ../Make.defines.solaris
EXTRA=-D_POSIX_PTHREAD_SEMANTICS
EXTRALIBS=-lrt -lpthread

PROGS =	atfork timeout suspend

all:	detach.o getenv1.o getenv2.o getenv3.o ${PROGS}

detach.o:	detach.c

timeout.o:	timeout.c

getenv1.o:	getenv1.c

getenv2.o:	getenv2.c

timeout:	timeout.o detach.o
		$(CC) $(CFLAGS) timeout.o detach.o $(LDFLAGS) $(LDLIBS) -o timeout

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
