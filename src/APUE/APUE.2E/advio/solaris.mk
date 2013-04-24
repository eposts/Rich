include ../Make.defines.solaris
EXTRA=

PROGS = catgetmsg devzero mcopy nonblockw pendlock

all:	${PROGS}

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
