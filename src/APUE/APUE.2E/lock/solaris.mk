include ../Make.defines.solaris
EXTRA=

PROGS =	deadlock mandatory

all:	${PROGS}

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
