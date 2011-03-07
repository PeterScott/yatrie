/* Simple trie, with two node types: a key-value pair, or a 256-ary branch. */
#include "yatrie.h"

/* Node types */
#define NODE_KV 1
#define NODE_BRANCH 2

/* Create a new, empty yatrie. */
yatrie_t yatrie_new(void) {
  return (yatrie_t)NULL;
}

