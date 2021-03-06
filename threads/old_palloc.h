#ifndef THREADS_OLD_PALLOC_H
#define THREADS_OLD_PALLOC_H

#include <stddef.h>
#include "threads/synch.h"

/* A memory pool. */
struct pool
  {
    struct lock lock;                   /* Mutual exclusion. */
    struct bitmap *used_map;            /* Bitmap of free pages. */
    uint8_t *base;                      /* Base of pool. */
  };

/* How to allocate pages. */
enum palloc_flags
  {
    PAL_ASSERT = 001,           /* Panic on failure. */
    PAL_ZERO = 002,             /* Zero page contents. */
    PAL_USER = 004              /* User page. */
  };

int old_palloc_init (size_t user_page_limit);
void *old_palloc_get_page (enum palloc_flags);
void *old_palloc_get_multiple (enum palloc_flags, size_t page_cnt);
void old_palloc_free_page (void *);
void old_palloc_free_multiple (void *, size_t page_cnt);

void init_pool (struct pool *, void *base, size_t page_cnt,
                       const char *name);
 bool page_from_pool (const struct pool *, void *page);
#endif /* threads/palloc.h */
