include ../Make.defines.linux

PROGS = buf tempfiles tempnam

all:	$(PROGS)

clean:
	rm -f $(PROGS)
