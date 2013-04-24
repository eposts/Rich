include ../Make.defines.macos
EXTRA=

PROGS =	deadlock mandatory

all:	${PROGS}

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
