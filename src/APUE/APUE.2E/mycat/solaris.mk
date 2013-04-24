include ../Make.defines.solaris

PROGS = mycat fgetsfputs getcputc

all:	$(PROGS)

clean:
	rm -f $(PROGS)
