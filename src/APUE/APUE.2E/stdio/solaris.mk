include ../Make.defines.solaris

PROGS = tempfiles tempnam

all:	$(PROGS)

clean:
	rm -f $(PROGS)
