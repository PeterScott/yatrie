CFLAGS = -g -Wall
all: sttest

sttest: sttest.o simpletrie.o yatrie.h
simpletrie.o: simpletrie.c yatrie.h

clean:
	rm -f *.o sttest
