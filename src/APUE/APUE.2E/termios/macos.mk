include ../Make.defines.macos
EXTRA=

PROGS =	csize settty t_getpass t_isatty t_raw t_ttyname winch

all:	${PROGS} ctermid.o

ctermid.o:	ctermid.c

getpass.o:	getpass.c

isatty.o:	isatty.c

ttyname.o:	ttyname.c

t_isatty:	t_isatty.o isatty.o

t_ttyname:	t_ttyname.o ttyname.o

t_getpass:	t_getpass.o getpass.o

clean:
	rm -f ${PROGS} ${TEMPFILES} *.o
