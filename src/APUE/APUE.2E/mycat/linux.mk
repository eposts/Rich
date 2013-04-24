include ../Make.defines.linux

PROGS = mycat fgetsfputs getcputc

all:	$(PROGS)

clean:
	rm -f $(PROGS)
