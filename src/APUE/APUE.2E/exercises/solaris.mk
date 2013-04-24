include ../Make.defines.solaris
EXTRA=

PROGS =	getpwsvr4 goodexit getlogin sizepipe

all:	${PROGS} openmax.o sleep.o sleepus_poll.o sleepus_select.o asyncsocket.o

goodexit.o:	goodexit.c

goodexit:	goodexit.o openmax.o
		$(CC) $(CFLAGS) -o goodexit goodexit.o openmax.o $(LDFLAGS) $(LDLIBS) -lpthread

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
