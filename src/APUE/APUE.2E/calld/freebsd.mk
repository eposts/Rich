include ../Make.defines.freebsd
EXTRA=

PROGS = calld
HDRS = calld.h

all:	${PROGS} 

childdial.o:		childdial.c ${HDRS}

cliargs.o:		cliargs.c ${HDRS}

client.o:		client.c ${HDRS}

ctlstr.o:		ctlstr.c ${HDRS}

debug.o:		debug.c ${HDRS}

devfile.o:		devfile.c ${HDRS}

dialfile.o:		dialfile.c ${HDRS}

expectstr.o:		expectstr.c ${HDRS}

lock.o:		lock.c ${HDRS}

loop.o:		loop.c ${HDRS}

main.o:		main.c ${HDRS}

request.o:		request.c ${HDRS}

sendstr.o:		sendstr.c ${HDRS}

sigchld.o:		sigchld.c ${HDRS}

sysfile.o:		sysfile.c ${HDRS}

ttydial.o:		ttydial.c ${HDRS}

ttyopen.o:		ttyopen.c ${HDRS}

calld:		childdial.o cliargs.o client.o ctlstr.o debug.o devfile.o dialfile.o \
		expectstr.o lock.o loop.o main.o request.o sendstr.o sigchld.o \
		sysfile.o ttydial.o ttyopen.o
		$(CC) $(CFLAGS) -o calld childdial.o cliargs.o client.o ctlstr.o \
			debug.o devfile.o dialfile.o expectstr.o lock.o loop.o \
			main.o request.o sendstr.o sigchld.o sysfile.o ttydial.o ttyopen.o \
			../sockets/clconn.o ../sockets/initsrv2.o $(LDFLAGS) $(LDLIBS)

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
