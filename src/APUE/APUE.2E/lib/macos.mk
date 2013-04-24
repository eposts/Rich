include ../Make.defines.macos

#
# Makefile for misc library.
#

AR	= ar
LIBMISC	= libapue.a
OBJS   = bufargs.o cliconn.o clrfl.o \
			 daemonize.o error.o errorlog.o lockreg.o locktest.o \
			 openmax.o pathalloc.o popen.o prexit.o prmask.o \
			 ptyfork.o ptyopen.o readn.o recvfd.o \
			 senderr.o sendfd.o servaccept.o servlisten.o \
			 setfl.o signal.o signalintr.o \
			 sleepus.o spipe.o \
			 tellwait.o ttymodes.o writen.o # sleep.o pathconf.o sysconf.o
RANLIB     = ranlib

all:	${OBJS}
	${AR} rv ${LIBMISC} $?
	${RANLIB} ${LIBMISC}

ptyopen.c:	bsd_ptyopen.c
		cp bsd_ptyopen.c ptyopen.c

${OBJS}:

clean:
	rm -f *.o a.out core temp.* ptyopen.c $(LIBMISC)
