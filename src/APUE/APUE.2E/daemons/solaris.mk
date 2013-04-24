include ../Make.defines.solaris
EXTRA=

all:	init.o lockfile.o reread.o reread2.o single.o

init.o:		init.c

lockfile.o:	lockfile.c

reread.o:	reread.c

reread2.o:	reread2.c

single.o:	single.c

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
