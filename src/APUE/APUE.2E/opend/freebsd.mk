include ../Make.defines.freebsd
EXTRA=

PROGS = opend.poll opend.select

all:	${PROGS}

client.o:	client.c

cliargs.o:	../opend.fe/cliargs.c
	$(CC) $(CFLAGS) -c ../opend.fe/cliargs.c

main.o:		main.c

request.o:	request.c

loop.poll.o:	loop.poll.c

loop.select.o:	loop.select.c

opend.poll:	main.o request.o cliargs.o client.o loop.poll.o
	$(CC) $(CFLAGS) -o opend.poll main.o cliargs.o client.o request.o loop.poll.o \
		$(LDFLAGS) $(LDLIBS)

opend.select:	main.o request.o cliargs.o client.o loop.select.o
	$(CC) $(CFLAGS) -o opend.select main.o cliargs.o client.o request.o loop.select.o \
		$(LDFLAGS) $(LDLIBS)

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
