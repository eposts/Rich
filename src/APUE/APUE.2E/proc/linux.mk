include ../Make.defines.linux

PROGS =	echoall echoarg exec1 exec2 fork1 fork2 pracct pruids shell1 shell2 \
	systest1 systest2 systest3 tellwait1 tellwait2 test1 times1 vfork1 vfork3 wait1 zombie

all:	$(PROGS) system.o

system.o:	system.c

systest1:	system.o systest1.o
		$(CC) $(CFLAGS) -o systest1 systest1.o system.o $(LDFLAGS) $(LDLIBS)

systest2:	system.o systest2.o
		$(CC) $(CFLAGS) -o systest2 systest2.o system.o $(LDFLAGS) $(LDLIBS)

pracct:		pracct.c
		$(CC) $(CFLAGS) -DHAS_AXSIG -DHAS_ACORE -o pracct pracct.c $(LDFLAGS) $(LDLIBS)

clean:
	rm -f $(PROGS) *.o
