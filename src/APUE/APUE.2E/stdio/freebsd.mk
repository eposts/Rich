include ../Make.defines.freebsd

PROGS = tempfiles tempnam

all:	$(PROGS)

clean:
	rm -f $(PROGS)
