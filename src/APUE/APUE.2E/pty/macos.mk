include ../Make.defines.macos

PROGS =	pty

all:	${PROGS}

loop.o:		loop.c

driver.o:	driver.c

main.o:		main.c

pty:		main.o loop.o driver.o
		gcc -o pty main.o loop.o driver.o $(LDDIR) -lapue

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
