include ../Make.defines.freebsd

PROGS = mycat fgetsfputs getcputc

all:	$(PROGS)

clean:
	rm -f $(PROGS)
