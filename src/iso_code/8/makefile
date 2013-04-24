#
# makefile for TINY
# Borland C Version
# K. Louden 2/3/98
#

CC = bcc

CFLAGS = 

OBJS = main.obj util.obj scan.obj parse.obj symtab.obj analyze.obj code.obj cgen.obj

tiny.exe: $(OBJS)
	$(CC) $(CFLAGS) -etiny $(OBJS)

main.obj: main.c globals.h util.h scan.h parse.h analyze.h cgen.h
	$(CC) $(CFLAGS) -c main.c

util.obj: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

scan.obj: scan.c scan.h util.h globals.h
	$(CC) $(CFLAGS) -c scan.c

parse.obj: parse.c parse.h scan.h globals.h util.h
	$(CC) $(CFLAGS) -c parse.c

symtab.obj: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.obj: analyze.c globals.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c analyze.c

code.obj: code.c code.h globals.h
	$(CC) $(CFLAGS) -c code.c

cgen.obj: cgen.c globals.h symtab.h code.h cgen.h
	$(CC) $(CFLAGS) -c cgen.c

clean:
	-del tiny.exe
	-del tm.exe
	-del main.obj
	-del util.obj
	-del scan.obj
	-del parse.obj
	-del symtab.obj
	-del analyze.obj
	-del code.obj
	-del cgen.obj
	-del tm.obj

tm.exe: tm.c
	$(CC) $(CFLAGS) -etm tm.c

tiny: tiny.exe

tm: tm.exe

all: tiny tm

