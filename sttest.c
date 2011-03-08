#include "yatrie.h"

int main(void) {
  yatrie_t trie;

  /* Insert values */
  trie = yatrie_new();
  trie = yatrie_insert(trie, 1, 11);
  trie = yatrie_insert(trie, 3, 33);
  trie = yatrie_insert(trie, 2, 22);
  trie = yatrie_insert(trie, 7777771234567891, 12345);
  
  /* Get values */
  word_t *val;
  val = yatrie_get(trie, 1);                printf("1 -> %llu\n", *val);
  val = yatrie_get(trie, 2);                printf("2 -> %llu\n", *val);
  val = yatrie_get(trie, 3);                printf("3 -> %llu\n", *val);
  val = yatrie_get(trie, 7777771234567891); printf("7777771234567891 -> %llu\n", *val);

  /* Clean up */
  yatrie_free(trie);
  return 0;
}
