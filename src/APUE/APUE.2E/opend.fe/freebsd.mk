include ../Make.defines.freebsd
EXTRA=

PROGS = opend

all:	${PROGS}

cliargs.o:	cliargs.c

main.o:		main.c

request.o:	request.c

opend:	main.o request.o cliargs.o
	$(CC) $(CFLAGS) -o opend main.o cliargs.o request.o $(LDFLAGS) $(LDLIBS)

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o

