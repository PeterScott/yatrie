/* Starting buffer size for nkv-node. In interval [3, 256] */
#define NKV_START_BUFSIZE 8
/* Growth factor for nkv-nodes. Factor = a/b, 0 < a <= 255, b is power
   of 2 between 1 and 128. */
#define NKV_GROWTH_FACTOR_A 2
#define NKV_GROWTH_FACTOR_B 1

/* Starting buffer size for ndp-node. In interval [3, 256] */
#define NDP_START_BUFSIZE 8
/* Growth factor for ndp-nodes. Factor = a/b, 0 < a <= 255, b is power
   of 2 between 1 and 128. */
#define NDP_GROWTH_FACTOR_A 2
#define NDP_GROWTH_FACTOR_B 1

/* Max K-V pairs in an nkv-node before it expands. Is upper bound, not
   inclusive. [3, 256] */
#define NKV_MAX_SIZE 128
/* Ditto, but for ndp-nodes. */
#define NDP_MAX_SIZE 32
