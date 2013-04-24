include ../Make.defines.solaris
EXTRA=

PROGS = strlist t_isastream

all:	${PROGS} spipe.o sendfd.o recvfd.o cliconn.o servlisten.o servaccept.o \
		ptyopen.o isastream.c

spipe.o:	spipe.c

sendfd.o:	sendfd.c

recvfd.o:	recvfd.c

isastream.o:	isastream.c

cliconn.o:	cliconn.c

servlisten.o:	servlisten.c

servaccept.o:	servaccept.c

ptyopen.o:	ptyopen.c

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o

