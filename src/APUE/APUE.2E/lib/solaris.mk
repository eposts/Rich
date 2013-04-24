include ../Make.defines.solaris
EXTRA=-D__EXTENSIONS__ -D_HAS_PTSNAME -D_HAS_GRANTPT -D_HAS_UNLOCKPT

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
			 sleep.o sleepus.o spipe.o \
			 tellwait.o ttymodes.o writen.o # pathconf.o sysconf.o strerror.o \
RANLIB     =

all:	${OBJS}
	${AR} rv ${LIBMISC} $?

ptyopen.c:	linux_ptyopen.c
		cp linux_ptyopen.c ptyopen.c

${OBJS}:

clean:
	rm -f *.o a.out core temp.* ptyopen.c $(LIBMISC)
