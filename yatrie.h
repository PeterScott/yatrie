#ifndef __YATRIE_H
#define __YATRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "config.h"
#include "params.h"

/************************** Miscellaneous utilities ***************************/

#define YATRIE_ERROR(format, ...) do {          \
    fprintf(stderr, format, __VA_ARGS__);       \
    exit(1);                                    \
  } while (0);

/************************** Low-level data mangling ***************************/

#define TAG_MASK 7

#ifdef ARCH_32BIT
typedef uint32_t word_t;

#define PTR_MASK 0xFFFFFFF8

#define MSB(word) (((word_t)(word) & 0xFF000000) >> 24)

// High and low half-word accessors
#define HIGH_HW(word) (((word_t)(word) & 0xFFFF0000) >> 16)
#define LOW_HW(word)  ((word_t)(word) & 0xFFFF)
#define PACK_WORD(high, low) (((word_t)(high) << 16) | (word_t)(low))
#endif

#ifdef ARCH_64BIT
typedef uint64_t word_t;

#define PTR_MASK 0xFFFFFFFFFFFFFFF8

#define MSB(word) (((word_t)(word) & 0xFF00000000000000) >> 56)

// High and low half-word accessors
#define HIGH_HW(word) (((word_t)(word) & 0xFFFFFFFF00000000) >> 32)
#define LOW_HW(word)  ((word_t)(word) & 0xFFFFFFFF)
#define PACK_WORD(high, low) (((word_t)(high) << 32) | (word_t)(low))
#endif



typedef word_t* yatrie_t;

#define TAG(ptr, tag) ((yatrie_t)((word_t)(ptr) | (word_t)(tag)))

#define SPLIT_TAG(ptr, tag) do {                      \
    tag = (word_t)ptr & (word_t)TAG_MASK;             \
    ptr = (yatrie_t)((word_t)ptr & (word_t)PTR_MASK); \
  } while (0);

/********************************* Yatrie API *********************************/

yatrie_t yatrie_new(void);
yatrie_t yatrie_insert(yatrie_t yatrie, word_t key, word_t value);
word_t *yatrie_get(yatrie_t yatrie, word_t key);
void yatrie_free(yatrie_t yatrie);
void yatrie_print(yatrie_t yatrie, int spaces);

#endif
