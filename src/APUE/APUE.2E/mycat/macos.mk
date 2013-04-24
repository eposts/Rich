include ../Make.defines.macos

PROGS = mycat fgetsfputs getcputc

all:	$(PROGS)

clean:
	rm -f $(PROGS)
