include ../Make.defines.linux

#
# Makefile for misc library.
#

AR	= ar
LIBMISC	= libapue.a
OBJS   = bufargs.o cliconn.o clrfl.o \
			 daemonize.o error.o errorlog.o lockreg.o locktest.o \
			 openmax.o pathalloc.o popen.o prexit.o prmask.o \
			 ptyfork.o linux_ptyopen.o readn.o recvfd.o \
			 senderr.o sendfd.o servaccept.o servlisten.o \
			 setfl.o signal.o signalintr.o \
			 sleep.o sleepus.o spipe.o \
			 tellwait.o ttymodes.o writen.o # pathconf.o sysconf.o strerror.o 

RANLIB     = ranlib

all:	${OBJS}
	${AR} rv ${LIBMISC} $?
	${RANLIB} ${LIBMISC}

ptyopen.c:	linux_ptyopen.c
		cp linux_ptyopen.c ptyopen.c

clean:
	rm -f *.o a.out core temp.* ptyopen.c $(LIBMISC)
