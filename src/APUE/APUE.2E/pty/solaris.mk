include ../Make.defines.solaris
EXTRA=-D__EXTENSIONS__
EXTRALIBS=-lsocket -lnsl

PROGS =	pty

all:	${PROGS}

loop.o:		loop.c

driver.o:	driver.c

main.o:		main.c

pty:		main.o loop.o driver.o
		$(LINK.c) -o pty main.o loop.o driver.o $(LDDIR) $(LDLIBS)

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
