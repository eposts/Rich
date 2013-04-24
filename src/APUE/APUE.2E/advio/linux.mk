include ../Make.defines.linux
EXTRA=

PROGS = catgetmsg devzero mcopy nonblockw pendlock

all:	${PROGS}

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
