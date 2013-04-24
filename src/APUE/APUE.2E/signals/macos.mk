include ../Make.defines.macos

PROGS =	critical mask read1 read2 reenter sigtstp \
	sigusr suspend1 suspend2 tsleep2

all:	abort.o sleep1.o sleep2.o system.o tsleep2.o ${PROGS}

abort.o:	abort.c

sleep1.o:	sleep1.c

sleep2.o:	sleep2.c

system.o:	system.c

newsystem.o:	newsystem.c

systest2.o:	systest2.c

tsleep2:	tsleep2.o sleep2.o
		gcc -o tsleep2 tsleep2.o sleep2.o $(LDDIR) -lapue

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
