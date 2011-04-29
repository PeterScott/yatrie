#include "yatrie.h"

int main(int argc, char **argv) {
  yatrie_t trie; int i, n;

  if (argc != 2) {
    printf("usage: %s n\n", argv[0]);
    exit(1);
  }

  n = atoi(argv[1]);

  /* Insert values */
  trie = yatrie_new();
  for (i = 0; i < n; i++)
    trie = yatrie_insert(trie, i, i + 1000000);
  
  /* Print the trie, for debugging */
  yatrie_print(trie, 0);

  /* Get values */
  word_t *val;
  for (i = 0; i < n; i++) {
    val = yatrie_get(trie, i);
    if (val == NULL || *val != (i + 1000000)) {
      printf("Error on %i\n", i);
      if (val == NULL) printf("val == NULL\n");
      else printf("*val = %llu\n", *val);
      exit(1);
    }
  }

  /* Clean up */
  yatrie_free(trie);
  printf("Success.\n");
  return 0;
}
