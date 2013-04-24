include ../Make.defines.macos
CFLAGS=-DLINUX -ansi -I$(WKDIR)/include -Wall

#
# Makefile for misc library.
#

AR	= ar
LIBMISC	= libapue_db.a
COMM_OBJ   = db.o
RANLIB     = ranlib

all: ${LIBMISC} t4

libapue_db.a:	${COMM_OBJ}
		${AR} rv ${LIBMISC} ${COMM_OBJ}
		${RANLIB} ${LIBMISC}

libapue_db.so.1:	db.c
		$(CC) -fPIC $(CFLAGS) -c db.c
		$(CC) -shared -Wl,-soname,libapue_db.so.1 -o libapue_db.so.1 -L../lib -lapue -lc db.o

t4:		libapue_db.a t4.o
		$(CC) -o t4 t4.o libapue_db.a -L../lib -lapue

clean:
	rm -f *.o a.out core temp.* $(LIBMISC) t4 libapue_db.so.*
