#ifndef VM_FRAME_H
#define VM_FRAME_H

#include "threads/synch.h"
#include <hash.h>
#include <bitmap.h>
#include "threads/thread.h"
#include "threads/old_palloc.h"
#include "string.h"

void frame_table_init(size_t num_frames);
void frame_table_free(void);

/* NOTE: NOT IMPLEMENTED. This data structure is not used in our code. */
struct sup_page_table
{
  struct hash all_pages;		// hash table of allocated pages
};

struct frame_table
{
  struct pool frame_pool;		// bitmap of free areas
  struct lock lock;			// for synchronization of the frame_table
  struct frame_entry * frames;		// array of frame_entry
};

// For supplemental page table. Not used.  
struct page_entry
{
  struct hash_elem pe_elem;		// elem to look for page in all_pages hash
  int f_num;				// frame number 
  int p_num;				// page number
  tid_t tid;				// tid of frame's process
};

// An element of the frame table. One per frame.
struct frame_entry
{
  tid_t pid;				// process pid
  int page_num;				// page number
  bool reference;			// if page was recently referenced
  bool dirty;				// if page has been recently written to
  bool resident;			// if the page is in the page table
  void * kpage;				// pointer to the user frame
  
};

//functions for hash_table
hash_less_func less;
hash_hash_func hash;

#endif /* vm/frame.h */
