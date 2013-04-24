include ../Make.defines.solaris

PROGS = conf options

all:	${PROGS}

conf:	conf.c

conf.c:
	nawk -f makeconf.awk >conf.c

options:	options.c

options.c:
	nawk -f makeopt.awk >options.c

clean:
	rm -f ${PROGS} ${TEMPFILES} options.c conf.c
