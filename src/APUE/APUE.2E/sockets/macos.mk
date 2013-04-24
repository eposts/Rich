include ../Make.defines.macos
EXTRA=

PROGS = findsvc bo ruptime ruptimed ruptimed-fd ruptime-dg ruptimed-dg \
	bindunix 

all:	${PROGS} clconn.o initsrv1.o initsrv2.o recvfd.o sendfd.o \
		cliconn.o servlisten.o servaccept.o spipe.o

clconn.o:	clconn.c

initsrv1.o:	initsrv1.c

initsrv2.o:	initsrv2.c

ruptime.o:	ruptime.c

ruptimed.o:	ruptimed.c

ruptimed-fd.o:	ruptimed-fd.c

ruptimed-dg.o:	ruptimed-dg.c

recvfd.o:	recvfd.c

sendfd.o:	sendfd.c

cliconn.o:	cliconn.c

servlisten.o:	servlisten.c

servaccept.o:	servaccept.c

spipe.o:	spipe.c

ruptime:	ruptime.o clconn.o
		$(CC) $(CFLAGS) -o ruptime ruptime.o clconn.o $(LDFLAGS) $(LDLIBS)

ruptimed:	ruptimed.o initsrv2.o
		$(CC) $(CFLAGS) -o ruptimed ruptimed.o initsrv2.o  \
			$(LDFLAGS) $(LDLIBS)

ruptimed-fd:	ruptimed-fd.o initsrv2.o
		$(CC) $(CFLAGS) -o ruptimed-fd ruptimed-fd.o initsrv2.o \
			$(LDFLAGS) $(LDLIBS)

ruptimed-dg:	ruptimed-dg.o initsrv2.o
		$(CC) $(CFLAGS) -o ruptimed-dg ruptimed-dg.o initsrv2.o \
			$(LDFLAGS) $(LDLIBS)

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
