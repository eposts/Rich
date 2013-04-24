include ../Make.defines.freebsd
EXTRA=

PROGS =	deadlock mandatory

all:	${PROGS}

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
