include ../Make.defines.freebsd
EXTRA=

PROGS = devzero mcopy nonblockw pendlock

all:	${PROGS}

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
