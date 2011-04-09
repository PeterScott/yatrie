CFLAGS = -g -Wall
HEADERS = yatrie.h config.h params.h
all: sttest ttest bigtest

sttest: sttest.o simpletrie.o $(HEADERS)
ttest: sttest.o trie.o $(HEADERS)
	gcc $^ -o $@
bigtest: bigtest.o trie.o $(HEADERS)

clean:
	rm -f *.o sttest
