#include "threads/malloc.h"
#include "vm/frame.h"
#include "threads/old_palloc.h"
#include "threads/vaddr.h"
#include "threads/palloc.h"

#define DBP false

struct frame_table frame_table;

/* As per its name, this function initializes the frame table, by
 * allocating all the physical user pages, assigning them to frames,
 * setting the frames to their default (unused) values, and initializing
 * the other frametable members, including a lock and a pool to regulate
 * it's use.
 */
void
frame_table_init(size_t num_frames)
{
   // initialize lock
   lock_init(&frame_table.lock);
  
   // allocate user pages
   void * base = old_palloc_get_multiple(PAL_USER | PAL_ZERO | PAL_ASSERT, num_frames);

   // intialize frame_pool
   init_pool(&frame_table.frame_pool, base, num_frames, "frame pool");

   // initialize frame table
   size_t i;
   frame_table.frames = malloc(sizeof(struct frame_entry)*num_frames);
   struct frame_entry * cur_frame;
   for(i = 0; i < num_frames; i++)
   {
      cur_frame = &frame_table.frames[i];
      cur_frame->pid = -1;
      cur_frame->page_num = -1;
      cur_frame->reference = false;
      cur_frame->dirty = false;
      cur_frame->resident = false;
      cur_frame->kpage = (frame_table.frame_pool.base + i * PGSIZE);
   }
}

/* Deallocates the frame table memory. */
void
frame_table_free()
{
   free(frame_table.frames);
}

/* A function to use with the supplemental page table. This is not called
 * anywhere in our code. 
 */
bool less(const struct hash_elem *elem_a, const struct hash_elem *elem_b, void * aux UNUSED)
{
   struct page_entry * pa = hash_entry(elem_a, struct page_entry, pe_elem);
   struct page_entry * pb = hash_entry(elem_b, struct page_entry, pe_elem);
   if(pa->tid == pb->tid)
   {
      if(pa->p_num < pb->p_num)
         return pa;
      else
         return pb;
   }
   else if(pa->tid < pb->tid)
      return pa;
   else
      return pb;
}

/* A function for use with the suplamental frame table. This function
 * is not called anywhere in our code. 
 */
unsigned hash(const struct hash_elem * elem, void * aux UNUSED)
{
   struct page_entry * page = hash_entry(elem, struct page_entry, pe_elem);
   return page->p_num + page->tid;
}

