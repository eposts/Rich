include ../Make.defines.freebsd

FILES= doatexit testjmp.opt testjmp

all:	$(FILES)

testjmp:	testjmp.c
		$(CC) $(CFLAGS) -o testjmp testjmp.c $(LDFLAGS) $(LDLIBS)

testjmp.opt:	testjmp.c
		$(CC) $(CFLAGS) -O -o testjmp.opt testjmp.c $(LDFLAGS) $(LDLIBS)

clean:
		rm -f *.o $(FILES)
