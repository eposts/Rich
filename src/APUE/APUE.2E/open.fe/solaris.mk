include ../Make.defines.solaris
EXTRA=
EXTRALIBS=-lsocket -lnsl

PROGS = openclient

all:	${PROGS}

main.o:		main.c

open.o:		open.c

openclient:	main.o open.o
	$(CC) $(CFLAGS) -o openclient main.o open.o $(LDFLAGS) $(LDLIBS)

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
