include ../Make.defines.macos

PROGS = buf tempfiles tempnam

all:	$(PROGS)

clean:
	rm -f $(PROGS)
