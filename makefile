CFLAGS = -g -Wall
HEADERS = yatrie.h config.h params.h
all: sttest ttest

sttest: sttest.o simpletrie.o $(HEADERS)
ttest: sttest.o trie.o $(HEADERS)
	gcc $^ -o $@

clean:
	rm -f *.o sttest
