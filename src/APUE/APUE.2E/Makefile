DIRS = lib sockets advio call calld daemons datafiles db environ exercises \
	file ipc ipp lock mycat open opend opend.fe open.fe proc pty \
	sess signals std stdio streams termios threadctl threads

all:
	$(MAKE) `./systype.sh`

clean:
	for i in $(DIRS); do \
		(cd $$i && $(MAKE) -f `../systype.sh`.mk clean) || exit 1; \
	done

freebsd:
	for i in $(DIRS); do \
		(cd $$i && $(MAKE) -f freebsd.mk) || exit 1; \
	done

linux:
	for i in $(DIRS); do \
		(cd $$i && $(MAKE) -f linux.mk) || exit 1; \
	done

macos:
	for i in $(DIRS); do \
		(cd $$i && $(MAKE) -f macos.mk) || exit 1; \
	done

solaris:
	for i in $(DIRS); do \
		(cd $$i && $(MAKE) -f solaris.mk) || exit 1; \
	done
