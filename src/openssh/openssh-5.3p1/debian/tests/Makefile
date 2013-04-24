test: getpid.so
	chmod +x keygen-test
	./keygen-test

getpid.o: getpid.c
	gcc -fPIC -c $< -o $@

getpid.so: getpid.o
	gcc -shared -o $@ $<

clean:
	rm -f getpid.o getpid.so key1 key1.pub key2 key2.pub
