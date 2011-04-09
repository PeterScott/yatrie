/* Simple trie, with two node types: a key-value pair, or a 256-ary branch. */
#include "yatrie.h"

/******************************** Declarations ********************************/

/* Node types */
#define NODE_KV      1          /* Key-value pair */
#define NODE_TWOKV   2          /* Two K-V pairs */
#define NODE_NKV     3          /* n K-V pairs */
#define NODE_NDP     4          /* n D-P pairs */
#define NODE_BRANCH  5          /* 256 pointers */

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
  int i = 0;

  /* Empty root: insert key-value pair. */
  if (yatrie == NULL) {
    printf("ins into empty\n");
    word_t *node = malloc(2 * sizeof(word_t));
    node[0] = key; node[1] = value;
    return TAG(node, NODE_KV);
  }

  /* Split off the tag bits, to determine node type. */
  int node_type; SPLIT_TAG(yatrie, node_type);

  /* Key-value pair: go to 2-KV pairs */
  if (node_type == NODE_KV) {
    printf("ins into kv %llx\n", (uint64_t)yatrie);
    word_t *node = malloc(4 * sizeof(word_t));
    node[0] = yatrie[0]; node[1] = yatrie[1];
    node[2] = key; node[3] = value;
    free(yatrie);
    return TAG(node, NODE_TWOKV);
  }

  /* Two k-v pairs: split into n-K-V */
  if (node_type == NODE_TWOKV) {
    printf("ins into 2kv %llu\n", (uint64_t)yatrie);
    /* First word is buffer length (high half-word) and fullness (low
       half-word). */
    word_t *node = calloc(NKV_START_BUFSIZE * 2 + 1, sizeof(word_t));
    node[0] = PACK_WORD(NKV_START_BUFSIZE, 3);
    node[1] = yatrie[0]; node[2] = yatrie[1];
    node[3] = yatrie[2]; node[4] = yatrie[3];
    node[5] = key; node[6] = value;
    free(yatrie);
    return TAG(node, NODE_NKV);
  }

  /* n K-V pairs: either append or split in various confusing ways. */
  if (node_type == NODE_NKV) {
    printf("ins into nkv %llu\n", (uint64_t)yatrie);
    /* Buffer length and number of K-V pairs currently here. */
    word_t buflen  = HIGH_HW(yatrie[0]);
    word_t kvpairs = LOW_HW(yatrie[0]);

    if ((kvpairs+1) < NKV_MAX_SIZE) {
      /* Do we need to resize? */
      if ((kvpairs+1) >= buflen) {
        word_t new_buflen = (buflen * NKV_GROWTH_FACTOR_A) / NKV_GROWTH_FACTOR_B;
        word_t *node = calloc(new_buflen * 2 + 1, sizeof(word_t));
        memcpy(node, yatrie, (buflen * 2 + 1) * sizeof(word_t));
        free(yatrie);
        yatrie = node; buflen = new_buflen;
      }

      yatrie[kvpairs * 2 + 1] = key;
      yatrie[kvpairs * 2 + 2] = value;
      kvpairs++;
      yatrie[0] = PACK_WORD(buflen, kvpairs);
      return TAG(yatrie, NODE_NKV);
    } else {
      /* Cheap hack: create an n-DP node, and insert each K-V pair
         into it with this function recursively. */
      word_t *node = calloc(NDP_START_BUFSIZE * 2 + 1, sizeof(word_t));
      node[0] = PACK_WORD(NDP_START_BUFSIZE, 0);
      yatrie_t subtrie = TAG(node, NODE_NDP);

      /* Insert each K-V pair into subtrie */
      for (i = 0; i < kvpairs; i++)
        subtrie = yatrie_insert_internal(subtrie, yatrie[2*i+1], shamt + 8, yatrie[2*i+2]);
      free(yatrie);
      subtrie = yatrie_insert_internal(subtrie, key, shamt + 8, value);
      return subtrie;
    }
  }

  /* n D-P pairs: either append or split to 256-ary node. */
  if (node_type == NODE_NDP) {
    printf("ins into ndp %llu\n", (uint64_t)yatrie);
    /* Buffer length and number of D-P pairs currently here. */
    word_t buflen  = HIGH_HW(yatrie[0]);
    word_t dppairs = LOW_HW(yatrie[0]);
    
    /* Look for an existing digit-pointer pair we can go with. */
    word_t digit = MSB(key << shamt);
    for (i = 0; i < dppairs; i++) {
      if (digit == yatrie[2*i+1]) {
        yatrie[2*i+2] = (word_t)yatrie_insert_internal((yatrie_t)yatrie[2*i+2], key, shamt + 8, value);
        return TAG(yatrie, NODE_NDP);
      }
    }

    /* If we're to the 256-ary threshold, convert. */
    if ((dppairs+1) >= NDP_MAX_SIZE) {
      word_t *node = calloc(256, sizeof(word_t));
      /* Array of pointers */
      for (i = 0; i < dppairs; i++) node[yatrie[2*i+1]] = yatrie[2*i+2];
      assert(node[digit] == 0);
      node[digit] = (word_t)yatrie_insert_internal(NULL, key, shamt + 8, value);
      free(yatrie);
      return TAG(node, NODE_BRANCH);
    }

    /* Reallocate if necessary */
    if ((dppairs+1) >= buflen) {
      word_t new_buflen = (buflen * NDP_GROWTH_FACTOR_A) / NDP_GROWTH_FACTOR_B;
      word_t *node = calloc(new_buflen * 2 + 1, sizeof(word_t));
      memcpy(node, yatrie, (buflen * 2 + 1) * sizeof(word_t));
      free(yatrie);
      yatrie = node; buflen = new_buflen;
    }

    /* Append D-P pair */
    yatrie[dppairs * 2 + 1] = digit;
    yatrie[dppairs * 2 + 1] = (word_t)yatrie_insert_internal(NULL, key, shamt + 8, value);
    return TAG(yatrie, NODE_BRANCH);
  }

  /* 256 digit-pointer pairs: recurse. */
  if (node_type == NODE_BRANCH) {
    printf("ins into branch %llu\n", (uint64_t)yatrie);
    int byte = MSB(key << shamt);
    yatrie[byte] = (word_t)yatrie_insert_internal((yatrie_t)yatrie[byte], key, shamt + 8, value);
    return TAG(yatrie, NODE_BRANCH);
  }

  YATRIE_ERROR("insert: Unknown node type: %i\n", node_type);
  return NULL;
}

/* Return a pointer to the value of the given key. You can modify this
   pointer. Returns NULL if the key was not found. */
word_t *yatrie_get(yatrie_t yatrie, word_t key) {
  int shamt = 0; int i = 0;

 beginning:

  /* Empty root: key not found. */
  if (yatrie == NULL) return NULL;

  /* Split off the tag bits, to determine node type. */
  int node_type; SPLIT_TAG(yatrie, node_type);

  /* Single key-value pair: look at key */
  if (node_type == NODE_KV) {
    printf("get kv\n");
    if (yatrie[0] == key) return yatrie+1;
    else return NULL;
  }

  /* Two K-V pairs: look at keys */
  if (node_type == NODE_TWOKV) {
    printf("get 2kv\n");
    for (i = 0; i < 2; i++)
      if (yatrie[2*i] == key) 
        return &yatrie[2*i+1];
    return NULL;
  }

  /* n K-V pairs: look at keys */
  if (node_type == NODE_NKV) {
    word_t kvpairs = LOW_HW(yatrie[0]);
    printf("get nkv [pairs=%i]\n", (int)kvpairs);

    for (i = 0; i < kvpairs; i++)
      if (yatrie[2*i+1] == key)
        return &yatrie[2*i+2];
    return NULL;
  }

  /* n D-P pairs: search and recurse. */
  if (node_type == NODE_NDP) {
    printf("get ndp\n");
    word_t dppairs = LOW_HW(yatrie[0]);
    int byte = MSB(key << shamt);

    for (i = 0; i < dppairs; i++) {
      if (yatrie[2*i+1] == byte) {
        yatrie = (yatrie_t)yatrie[2*i+2]; shamt += 8;
        goto beginning;
      }
    }
    return NULL;
  }    

  /* 256-ary branch: recurse. */
  if (node_type == NODE_BRANCH) {
    printf("get branch\n");
    int byte = MSB(key << shamt);
    yatrie = (yatrie_t)yatrie[byte]; shamt += 8;
    goto beginning;
  }

  YATRIE_ERROR("get: Unknown node type: %i\n", node_type);
  return NULL;
}

void yatrie_free(yatrie_t yatrie) {
  /* Empty root: nothing to free. */
  if (yatrie == NULL) return;

  /* Split off the tag bits, to determine node type. */
  int node_type; SPLIT_TAG(yatrie, node_type);

  /* Single key-value pair: just free it. */
  if (node_type == NODE_KV) {
    free(yatrie); return;
  }

  /* 256-ary branch: recurse. */
  if (node_type == NODE_BRANCH) {
    int i;
    for (i = 0; i < 256; i++)
      yatrie_free((yatrie_t)yatrie[i]);
    free(yatrie);
    return;
  }

  YATRIE_ERROR("free: Unknown node type: %i\n", node_type);
}
