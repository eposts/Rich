include ../Make.defines.macos

PROGS = conf options

all:	${PROGS}

conf:	conf.c

conf.c:
	awk -f makeconf.awk >conf.c

options:	options.c

options.c:
	awk -f makeopt.awk >options.c

clean:
	rm -f ${PROGS} ${TEMPFILES} options.c conf.c
