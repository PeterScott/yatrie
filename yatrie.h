#ifndef __YATRIE_H
#define __YATRIE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "config.h"

/*************************** Machine word data type ***************************/

#ifdef ARCH_32BIT
typedef uint32_t Word_t;

#define SPLIT_TAG(ptr, tag) do {                    \
    tag = (Word_t)ptr & (Word_t)7;                  \
    ptr = (Word_t)ptr & (Word_t)0xFFFFFFF8;         \
  } while (0);
#endif

#ifdef ARCH_64BIT
typedef uint64_t Word_t;

#define SPLIT_TAG(ptr, tag) do {                    \
    tag = (Word_t)ptr & (Word_t)7;                  \
    ptr = (Word_t)ptr & (Word_t)0xFFFFFFFFFFFFFFF8; \
  } while (0);
#endif

typedef Word_t yatrie_t;

/********************************* Yatrie API *********************************/

yatrie_t yatrie_new(void);

#endif
