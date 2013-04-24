include ../Make.defines.macos
EXTRA=

PROGS = devzero mcopy nonblockw pendlock

all:	${PROGS}

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
