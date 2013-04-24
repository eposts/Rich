include ../Make.defines.freebsd
EXTRA=
EXTRALIBS=-pthread

PROGS =	threadid exitstatus cleanup badexit2

all:	condvar.o mutex1.o mutex2.o mutex3.o rwlock.o ${PROGS}

condvar.o:	condvar.c

mutex1.o:	mutex1.c

mutex2.o:	mutex2.c

mutex3.o:	mutex3.c

rwlock.o:	rwlock.c

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o

