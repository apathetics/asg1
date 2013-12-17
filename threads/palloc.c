/* NOTE: THIS IS OUR CODE!!!! This is essentially a new file. The code 
 * that was in this file has all been moved to old_palloc.c. This is now 
 * a wrapper class that interacts with the frame table (found in frame.c). 
 * The idea was that the existing calls to the palloc methods could go
 * unchanged throughout the program. Unfortuantely, this ideal wasn't 
 * realize. So, we ended up using the calls to palloc in kernel mode as
 * the orignial functions, but all the calls to palloc in user mode use
 * other versions (palloc_page, palloc_multiple, palloc_free_upage, 
 * palloc_free_umultiple). So, these functions can only be used by the
 * code in user mode. The others must only be used in kernel mode. This 
 * is encapsulates the workings, and even the existance, of the frame
 * table from the code.
 */

#include "threads/palloc.h"
#include <bitmap.h>
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "threads/loader.h"
#include "threads/synch.h"
#include "threads/vaddr.h"
#include "threads/thread.h"
#include "userprog/pagedir.h"
#include "vm/frame.h"
extern struct frame_table frame_table;
#define DBG false

/* Initializes all the pages of physical memory, divides them evenly into
   into kernel and user pages. Also intializes the frame table, which grabs
   all of the user pages. All user pages must be obtained through the virtual
   memory construct. */
void
palloc_init (size_t user_page_limit)
{
   /* calculates the number of pages available
      to allocate after the bitmap is initialized */
   int num_user_pages = old_palloc_init(user_page_limit); 
   size_t bm_pages = DIV_ROUND_UP (bitmap_buf_size (num_user_pages), PGSIZE);
   frame_table_init(num_user_pages - bm_pages);
}

/* Obtains and assignes a group of contiguous upages to contiguous physical
   pages. PAL_USER must be set, and the pages will be obtained from virtual 
   memory, otherwise should panic kernel. If PAL_ZERO is set in FLAGS,
   then the pages are filled with zeros.  If too few pages are available, 
   returns false, unless PAL_ASSERT is set in FLAGS, in which case the kernel 
   panics. Returns the success value of this operation.*/
bool
palloc_multiple (enum palloc_flags flags, size_t page_cnt, 
                 void * upage, bool writable)
{ 
   bool success = false;
   if(!(flags & PAL_USER))
     PANIC ("palloc_multiple cannot be called without PAL_USER flag set\n");
 
   // can't allocate zero pages
   if (page_cnt == 0)
      return false;

   // allocate pages (by linking to phyiscal frames!)
   size_t i;
   for(i = 0; i < page_cnt; i++)
   {

      success = palloc_page(flags, upage + i * PGSIZE, writable);
      if(DBG) printf("success value for page index %d = %d\n", i, success);
      if(!success)
         break;
   }

   // clean up if allocation not entirely successful
   if(!success) 	
   {
      // add code to deallocate all allocated pages for this process
      // and unflip bits in pool, e.g. bitmpa_flip(page_idx, page_cnt);

      // execute flags
      if (flags & PAL_ASSERT)
         PANIC ("palloc_get: out of pages");
   }

   return success;
}

/* Obtains and returns a set of contiguous kernel pages from physical memory. 
   PAL_USER must NOT be set, and the pages will be obtained from virtual 
   memory, otherwise should panic kernel. If PAL_ZERO is set in FLAGS,
   then the pages are filled with zeros.  If too few pages are available, 
   returns NULL, unless PAL_ASSERT is set in FLAGS, in which case the kernel 
   panics. Returns pointer to the first address of contiguous memory.*/
void *
palloc_get_multiple (enum palloc_flags flags, size_t page_cnt)
{
   ASSERT(!(flags & PAL_USER));
   return old_palloc_get_multiple (flags, page_cnt);
}

/* Obtains a single free page and returns its kernel virtual
   address.
   If PAL_USER is set, the page is obtained from the user pool,
   otherwise from the kernel pool.  If PAL_ZERO is set in FLAGS,
   then the page is filled with zeros.  If no pages are
   available, returns a null pointer, unless PAL_ASSERT is set in
   FLAGS, in which case the kernel panics. */
void *
palloc_page (enum palloc_flags flags, void * upage, bool writable) 
{
   bool success = false;
   if(!(flags & PAL_USER))
   PANIC ("This function cannot be called without PAL_USER flag set\n");

   struct pool *pool = &frame_table.frame_pool;
   size_t page_idx;

   // empty page, and set to used
   lock_acquire (&pool->lock);
   page_idx = bitmap_scan_and_flip (pool->used_map, 0, 1, false);
   lock_release (&pool->lock);

   /* sets the members of cur_frame struct after is it allocated */
   struct frame_entry * cur_frame;
   cur_frame = frame_table.frames + page_idx;
   cur_frame->pid = thread_current()->tid;
   cur_frame->page_num = pg_no(upage);
   cur_frame->reference = true;  
   cur_frame->dirty = false;
   cur_frame->resident = true;
   success = install_page (upage, cur_frame->kpage, writable);

   if(DBG)printf("success after in mapping upage %p to kpage %p in palloc page = %d\n", upage, cur_frame->kpage, success);

   // execute flags
   if(!success)
   {
      if (flags & PAL_ASSERT)
         PANIC ("palloc_get: out of pages"); // change this?
      return NULL;
   }
   else if (flags & PAL_ZERO)
      memset (upage, 0, PGSIZE);

   return cur_frame->kpage;
}

/* Obtains a single free page and returns its kernel virtual
   address.
   If PAL_USER is set, the page is obtained from the user pool,
   otherwise from the kernel pool.  If PAL_ZERO is set in FLAGS,
   then the page is filled with zeros.  If no pages are
   available, returns a null pointer, unless PAL_ASSERT is set in
   FLAGS, in which case the kernel panics. */
void *
palloc_get_page (enum palloc_flags flags) 
{
  return old_palloc_get_multiple (flags, 1);
}

/* Frees the PAGE_CNT pages starting at PAGES. */
void
palloc_free_multiple (void *pages, size_t page_cnt) 
{
  old_palloc_free_multiple(pages, page_cnt);
}

/* Frees the page at PAGE. */
void palloc_free_upage(void * page)
{
  palloc_free_umultiple(page, 1);
}

/* Frees the PAGE_CNT pages starting at PAGES. */
void palloc_free_umultiple(void *pages, size_t page_cnt) 
// NOTE: THIS FUNCTION IS NOT CORRECT!!!
{
  size_t i;
  for(i = 0; i < page_cnt; i++)
  {
     void * kpage =lookup_page(thread_current()->pagedir, pages + i * PGSIZE, false);
     palloc_free_page(kpage);
  }
}

/* Frees the page at PAGE. */
void
palloc_free_page (void *page) 
{
  old_palloc_free_multiple (page, 1);
}

/* Adds a mapping from user virtual address UPAGE to kernel
   virtual address KPAGE to the page table.
   If WRITABLE is true, the user process may modify the page;
   otherwise, it is read-only.
   UPAGE must not already be mapped.
   KPAGE should probably be a page obtained from the user pool
   with palloc_get_page().
   Returns true on success, false if UPAGE is already mapped or
   if memory allocation fails. */
bool
install_page (void *upage, void *kpage, bool writable)
{
  struct thread *t = thread_current ();

  /* Verify that there's not already a page at that virtual
     address, then map our page there. */

  bool pageset = false;
  bool isnull = (pagedir_get_page (t->pagedir, upage) == NULL);
  
  if(isnull)
     pageset = pagedir_set_page (t->pagedir, upage, kpage, writable);

if(DBG)printf("pageset boolean in install page = %d\n", pageset);
if(DBG)printf("isnull boolean in install page = %d\n", isnull);

  return (isnull && pageset);
}

