/* Simple trie, with two node types: a key-value pair, or a 256-ary branch. */
#include "yatrie.h"

/******************************** Declarations ********************************/

/* Node types */
#define NODE_KV 1
#define NODE_BRANCH 2

yatrie_t yatrie_insert_internal(yatrie_t yatrie, word_t key, int shamt, word_t value);


/********************** Yatrie-compatible implementation **********************/

/* Create a new, empty yatrie. */
yatrie_t yatrie_new(void) {
  return (yatrie_t)NULL;
}

/* Insert a key, value pair into a yatrie, and return its new value. */
yatrie_t yatrie_insert(yatrie_t yatrie, word_t key, word_t value) {
  return yatrie_insert_internal(yatrie, key, 0, value);
}


/* Like yatrie_insert, but takes an additional argument whose top byte is the
   byte to use at the current level in the trie insertion process. */
yatrie_t yatrie_insert_internal(yatrie_t yatrie, word_t key, int shamt, word_t value) {

  /* Empty root: insert key-value pair. */
  if (yatrie == NULL) {
    word_t *node = malloc(2 * sizeof(word_t));
    node[0] = key; node[1] = value;
    return TAG(node, NODE_KV);
  }

  /* Split off the tag bits, to determine node type. */
  int node_type; SPLIT_TAG(yatrie, node_type);

  /* Single key-value pair: split into 256-ary branch. */
  if (node_type == NODE_KV) {
    word_t *branch = calloc(256, sizeof(word_t));
    word_t k = yatrie[0], v = yatrie[1]; int byte = MSB(k << shamt);
    branch[byte] = (word_t)yatrie_insert_internal((yatrie_t)branch[byte], k, shamt + 8, v);
    byte = MSB(key << shamt);
    branch[byte] = (word_t)yatrie_insert_internal((yatrie_t)branch[byte], key, shamt + 8, value);
    free(yatrie);
    return TAG(branch, NODE_BRANCH);
  }

  /* 256-ary branch: recurse. */
  if (node_type == NODE_BRANCH) {
    int byte = MSB(key << shamt);
    yatrie[byte] = (word_t)yatrie_insert_internal((yatrie_t)yatrie[byte], key, shamt + 8, value);
    return TAG(yatrie, NODE_BRANCH);
  }

  YATRIE_ERROR("Unknown node type: %i\n", node_type);
  return NULL;
}

/* Return a pointer to the value of the given key. You can modify this
   pointer. Returns NULL if the key was not found. */
word_t *yatrie_get(yatrie_t yatrie, word_t key) {
  int shamt = 0;

 beginning:

  /* Empty root: key not found. */
  if (yatrie == NULL) return NULL;

  /* Split off the tag bits, to determine node type. */
  int node_type; SPLIT_TAG(yatrie, node_type);

  /* Single key-value pair: look at key */
  if (node_type == NODE_KV) {
    if (yatrie[0] == key) return yatrie+1;
    else return NULL;
  }

  /* 256-ary branch: recurse. */
  if (node_type == NODE_BRANCH) {
    int byte = MSB(key << shamt);
    yatrie = (yatrie_t)yatrie[byte]; shamt += 8;
    goto beginning;
  }

  YATRIE_ERROR("Unknown node type: %i\n", node_type);
  return NULL;
}
