include ../Make.defines.solaris
EXTRA=

OBJ = getpwnam.o

all:	${OBJ}

clean:
	rm -f ${OBJ} ${TEMPFILES} *.o
