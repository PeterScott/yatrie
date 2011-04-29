/* Benchmarking code. */

#include "yatrie.h"
#include "benchmark.h"

void run_benchmarks(void) {
  int asizes[] = {2, 5, 10, 30, 500};
  int i, j;
  yatrie_t trie = (yatrie_t)NULL;
  int set_time = 0; int get_time = 0;

  BENCHMARK_INIT();

  for (i = 0; i < 5; i++) {
    int array_size = asizes[i];
    
    /* Contiguous keys */
    TICK();
    for (j = 0; j < array_size; j++)
      trie = yatrie_insert(trie, j, j);
    TOCK();
    set_time += benchmark_total_time * 500 / array_size;

    TICK();
    for (j = 0; j < array_size; j++)
      yatrie_get(trie, j);
    TOCK();
    get_time += benchmark_total_time * 500 / array_size;
    yatrie_free(trie); trie = (yatrie_t)NULL;

    /* Uniform keys */
    srand(1234567);
    TICK();
    for (j = 0; j < array_size; j++)
      trie = yatrie_insert(trie, rand(), j);
    TOCK();
    set_time += benchmark_total_time * 500 / array_size;

    TICK();
    for (j = 0; j < array_size; j++)
      yatrie_get(trie, rand());
    TOCK();
    get_time += benchmark_total_time * 500 / array_size;
    yatrie_free(trie); trie = (yatrie_t)NULL;
  }
  
  printf("%i %i\n", get_time, set_time);
}

int main(void) {
  run_benchmarks();
  return 0;
}
