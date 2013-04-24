include ../Make.defines.linux
EXTRA=

OBJ = getpwnam.o

all:	${OBJ}

clean:
	rm -f ${OBJ} ${TEMPFILES} *.o
