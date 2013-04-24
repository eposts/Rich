include ../Make.defines.solaris

#
# Makefile for misc library.
#

AR	= ar
LIBMISC	= libapue_db.a
COMM_OBJ   = db.o

all: ${LIBMISC} libapue_db.so.1 t4

libapue_db.a:	${COMM_OBJ}
		${AR} rv ${LIBMISC} ${COMM_OBJ}

libapue_db.so.1:	db.c
		$(CC) -fPIC $(CFLAGS) -c db.c
		$(CC) -shared -o libapue_db.so.1 -L../lib -lapue -lc db.o

#
# NB:
# set LD_LIBRARY_PATH to the directory containing libapue_db.so.1 to run t4
#
t4:		libapue_db.a t4.o
		$(CC) $(CFLAGS) -I. -c t4.c
		$(CC) -o t4 t4.o libapue_db.so.1 -L../lib -lapue

clean:
	rm -f *.o a.out core temp.* $(LIBMISC) t4 libapue_db.so.*
