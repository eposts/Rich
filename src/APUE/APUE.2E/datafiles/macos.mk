include ../Make.defines.macos
EXTRA=

OBJ = getpwnam.o

all:	${OBJ}

clean:
	rm -f ${OBJ} ${TEMPFILES} *.o
