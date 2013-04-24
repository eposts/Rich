include ../Make.defines.freebsd
EXTRA=

OBJ = getpwnam.o

all:	${OBJ}

clean:
	rm -f ${OBJ} ${TEMPFILES} *.o
