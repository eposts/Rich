include ../Make.defines.freebsd
EXTRA=

PROGS = add2 add2stdio fifo1 myuclc pipe1 pipe2 pipe4 popen1 popen2 spipe4 tshm

all:	${PROGS} tellwait.o

tellwait.o:	tellwait.c

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
