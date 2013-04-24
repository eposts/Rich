include ../Make.defines.linux
EXTRA=

PROGS =	deadlock mandatory

all:	${PROGS}

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
