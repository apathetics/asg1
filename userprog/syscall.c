#include "userprog/syscall.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "devices/input.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/synch.h"
#include "threads/malloc.h"
#include "threads/vaddr.h"

/* Our defines */
#define NUM_SYSCALLS 20
#define SYSCALL_LOWER SYS_HALT
#define SYSCALL_UPPER SYS_INUMBER
#define DBP false

/* A lock for synchronizatio of file system access. 
 * Declared in process.h */
extern struct lock file_lock;

static void syscall_handler (struct intr_frame *);
bool valid_ptr(const void * ptr);

/* Table for system call function lookeup */
syscall_wrapper* handler_table[NUM_SYSCALLS];

void
syscall_init (void) 
{
  lock_init(&file_lock);

  // initializes handler table for table lookup of sys calls
  handler_table[SYS_HALT] = halt_w;
  handler_table[SYS_EXIT] = exit_w;
  handler_table[SYS_EXEC] = exec_w;
  handler_table[SYS_WAIT] = wait_w;
  handler_table[SYS_CREATE] = create_w;
  handler_table[SYS_REMOVE] = remove_w;
  handler_table[SYS_OPEN] = open_w;
  handler_table[SYS_FILESIZE] = filesize_w;
  handler_table[SYS_READ] = read_w;
  handler_table[SYS_WRITE] = write_w;
  handler_table[SYS_SEEK] = seek_w;
  handler_table[SYS_TELL] = tell_w;
  handler_table[SYS_CLOSE] = close_w;

  // need to fix assignments for later projects
  handler_table[SYS_MMAP] = NULL;
  handler_table[SYS_MUNMAP] = NULL;
  handler_table[SYS_CHDIR] = NULL;
  handler_table[SYS_MKDIR] = NULL;
  handler_table[SYS_READDIR] = NULL;
  handler_table[SYS_ISDIR] = NULL;
  handler_table[SYS_INUMBER] = NULL;

  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* Our handler function to appropriately call the system call function
 * We use function pointers to call the correct system call. 
 * Every system call is passed 4 arguments, for simplicty, but each system call
 * function will ignore any extra arguments passed and their values will not be
 * altered. Also, most system calls will "return" a value to eax. Since eax is
 * caller save, this does not cause a probelm either. */
static void
syscall_handler (struct intr_frame *f) 
{
  uint32_t call_num, arg1, arg2, arg3;
  
  if(DBP) printf ("system call!\n");

  void * esp = f->esp;
  /* Check that esp is valid, kill thread otherwise. */
  struct thread *cur = thread_current();
  if(esp == NULL || is_kernel_vaddr(esp) || pagedir_get_page(cur->pagedir, esp) == NULL)
     thread_exit();

  /* Get system call_number */
  memcpy(&call_num, esp, 4);
  if(call_num > SYSCALL_UPPER)
     thread_exit(); 
 
  /* Get remaining 3 arguments, even if call doesn't require them */
  memcpy(&arg1, (esp + 4), 4);
  memcpy(&arg2, (esp + 8), 4);
  memcpy(&arg3, (esp + 12), 4);

  /* Make appropriate system */
  uint32_t result = handler_table[call_num](arg1, arg2, arg3);
  memcpy(&f->eax, &result , 4);
}

//uses shutdown_power_off() from devices/shutdown.h
uint32_t halt_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   shutdown_power_off();
   return 0;
}

//current user program terminates, and closes all the files
uint32_t exit_w(uint32_t arg1, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
if(DBP)printf("entering exit_w\n");
   int status = (int) arg1;
   struct thread *cur = thread_current();

   /* set status and print, if valid */
   if(cur->tid_node_exists)
   {
      struct tid_status *cur_tid_node = cur->tid_node;
      cur_tid_node->exit_status = status;
   }
   if(status >= 0)
      printf("%s: exit(%d)\n", thread_name(), status);

   thread_exit();
   return status;
} 

//checks for valid pointer and calls process_exec
uint32_t exec_w(uint32_t arg1, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   const char *cmd_line = (char*) arg1;
   if(!valid_ptr((void*)cmd_line) )
   {
      thread_exit();
      return -1;
   }
   return process_execute(cmd_line);
} 

// calls process_wait on the pid passed in
uint32_t wait_w(uint32_t arg1, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   tid_t pid = (tid_t) arg1;
   return process_wait(pid);
}
 
//error checks the parameters and creates a new file       
uint32_t create_w(uint32_t arg1, uint32_t arg2, uint32_t arg3 UNUSED)
{
   const char *file_name = (char *) arg1;
   unsigned int initial_size = (unsigned int) arg2;
   bool result;

   /* verify file_name is valid */
   if(!valid_ptr(file_name) || strlen(file_name) == 0)
      thread_exit();

   /* create the file */
   lock_acquire(&file_lock);
   result = filesys_create(file_name, initial_size);
   lock_release(&file_lock);
   return result;
}
        
//if file is open leave it open until it is done being used then close it.
//else remove it with filesys_remove  
uint32_t remove_w(uint32_t arg1, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   const char *file_name = (char *) arg1;
   bool result;
 
   /* verify file_name is valid */
   if(!valid_ptr(file_name))
      thread_exit();
   if(strlen(file_name) == 0)
      return -1;

   /* remove the file */
   lock_acquire(&file_lock);
   result = filesys_remove(file_name);
   lock_release(&file_lock);
   return result;
}

//checks the parameters and then opens the file and assigns it a fd
// we construct an fd_elem to store all the file and its related info        
uint32_t open_w(uint32_t arg1, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   const char* file_name = (char *) arg1;
   uint32_t file_size;

   /* verify file_name is valid */
   if(!valid_ptr(file_name))
      thread_exit();
   if(strlen(file_name) == 0)
      return -1;

   // try to open file name
   lock_acquire(&file_lock);
   struct file *the_file = filesys_open (file_name);
   if(the_file == NULL)
   {
      lock_release(&file_lock);
      return -1;
   }
   file_size = file_length(the_file);
   lock_release(&file_lock); 

   /* create file descriptor node */
   struct thread *cur = thread_current(); 
   struct fd_elem *fd_cur = malloc(sizeof(struct fd_elem));
   fd_cur->filename = malloc(15*sizeof(char));
   strlcpy(fd_cur->filename, file_name, 15);
   fd_cur->the_file = the_file;
   fd_cur->fd = (cur->next_fd)++;
   fd_cur->file_size = file_size;

   /* add file descriptor node to fd_list */
   list_push_back(&cur->fd_list, &fd_cur->elem);

   return fd_cur->fd;
}

//returns file size, if it cannot find fd -1 is returned
uint32_t filesize_w(uint32_t arg1, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   int fd = (int) arg1;
   struct thread *cur = thread_current();
   
   /* check if fd has even been assigned to a file */
   if(fd < 2 || fd >= cur->next_fd)
      return -1;

   /* search for file in fd_list */
   struct list_elem *e;   
   for (e = list_begin (&cur->fd_list); e != list_end (&cur->fd_list);
        e = list_next (e))
   {
        struct fd_elem *cur_file_elem = list_entry (e, struct fd_elem, elem);
        if(cur_file_elem->fd == fd)
        {
           /* return the file_size if found */
	   return cur_file_elem->file_size;
        }
   }
   /* only reaches this code if fd not in list */
   return -1;
}
        
// checks the parameters.  if fd = 0, then interrupts are turned off 
// and input_getc is used.  if fd is between 2 and 128, then we find
// the file and read it
uint32_t read_w(uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
   int fd = (int) arg1;
   void *buffer = (void *) arg2;
   unsigned int size = (unsigned int) arg3;
   uint32_t result;

   /* verify buffer pointer is valid */
   if(!valid_ptr(buffer))
      thread_exit();  

   if(fd == 0) // read from stdin
   {  
      enum intr_level old_level;
      old_level = intr_disable ();
      result = (uint32_t)input_getc();
      intr_set_level (old_level);
   }
   else // attempt to read from a file
   {
      struct thread *cur = thread_current();  

      /* return if fd not assigned to a file */
      if(fd < 2 || fd >= cur->next_fd)
         return -1;

      /* search for file in fd_list */
      struct list_elem *e;
      for (e = list_begin (&cur->fd_list); e != list_end (&cur->fd_list);
           e = list_next (e))
      {
           struct fd_elem *cur_file_elem = list_entry (e, struct fd_elem, elem);
           if(cur_file_elem->fd == fd)
           {
              /* read from the file if found */
	      lock_acquire(&file_lock);   
	      result = (uint32_t) file_read (cur_file_elem->the_file, buffer, size); 
	      lock_release(&file_lock);
	      return result;
           }
      }   
      /* return error code if fd not in list */
      result = -1;
   }
   return result;
}
 
// checks parameters and if fd == 1, writes to console
// if fd is between 2 and 128, then the contents are written
// to the file
uint32_t write_w(uint32_t arg1, uint32_t arg2, uint32_t arg3)
{
   int fd = (int) arg1;
   const void *buffer = (void *) arg2;
   unsigned int size = (unsigned int) arg3;
   uint32_t result;
 
   /* verify buffer pointer is valid */
   if(!valid_ptr(buffer))
      thread_exit();

   if(fd == 1) // write to stdout
   {  
      printf("%s", (char*)buffer);
      return size;
   }
   else // attempt to write to a file
   {
      struct thread *cur = thread_current(); 

      /* return if fd not assigned to a file */
      if(fd < 2 || fd >= cur->next_fd)
         return -1;

      /* search for file in fd_list */
      struct list_elem *e;
      for (e = list_begin (&cur->fd_list); e != list_end (&cur->fd_list);
         e = list_next (e))
      {
         struct fd_elem *cur_file_elem = list_entry (e, struct fd_elem, elem);
         if(cur_file_elem->fd == fd)
         {
            /* write to the file if found */
	    lock_acquire(&file_lock);   
	    result = (uint32_t) file_write (cur_file_elem->the_file, buffer, size); 
	    lock_release(&file_lock);
	    return result;
         }
      }
      /* return error code if fd not in list */
      result = -1;
   }
   return result;
}
        
//find the fd_elem from fd and seeks for the position in that file
uint32_t seek_w(uint32_t arg1, uint32_t arg2, uint32_t arg3 UNUSED)
{
   int fd = (int) arg1;
   unsigned int position = (unsigned int) arg2; 
   struct thread *cur = thread_current();  

  /* check if fd has even been assigned to a file */
   if(fd < 2 || fd >= cur->next_fd)
      return -1;

   /* search for file in fd_list */
   struct list_elem *e;
   for (e = list_begin (&cur->fd_list); e != list_end (&cur->fd_list);
        e = list_next (e))
   {
        struct fd_elem *cur_file_elem = list_entry (e, struct fd_elem, elem);
        if(cur_file_elem->fd == fd)
        {
           /* move to given position, if fd found */
	   lock_acquire(&file_lock);
	   file_seek(cur_file_elem->the_file, position);
	   lock_release(&file_lock);
	   break;
        }
   }
   /* only reaches this code if fd not in list */
   return -1;
}
        
//finds the fd_elem from fd and tells the position in that file
uint32_t tell_w(uint32_t arg1, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   int fd = (int) arg1;   
   uint32_t result;
   struct thread *cur = thread_current();  

  /* check if fd has even been assigned to a file */
   if(fd < 2 || fd >= cur->next_fd)
      return -1;

   /* search for file in fd_list */
   struct list_elem *e;
   for (e = list_begin (&cur->fd_list); e != list_end (&cur->fd_list);
        e = list_next (e))
   {
        struct fd_elem *cur_file_elem = list_entry (e, struct fd_elem, elem);
        if(cur_file_elem->fd == fd)
        {
           /* return the file position, if fd found */
	   lock_acquire(&file_lock);
	   result = (uint32_t) file_tell(cur_file_elem->the_file);
	   lock_release(&file_lock);
           return result;
        }
   }
   /* only reaches this code if fd not in list */
   return -1;
}
        
//finds the fd_elem from fd and closes the file and removes it from our
//fd_list
uint32_t close_w(uint32_t arg1, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   int fd = (int) arg1;
   struct thread *cur = thread_current(); 

   /* check if fd has even been assigned to a file */ 
   if(fd < 2 || fd >= cur->next_fd)
      return -1; // this value doesn't matter

   /* search for file in fd_list */
   struct list_elem *e;
   for (e = list_begin (&cur->fd_list); e != list_end (&cur->fd_list);
        e = list_next (e))
   {
        struct fd_elem *cur_file_elem = list_entry (e, struct fd_elem, elem);
        if(cur_file_elem->fd == fd)
        {
           /*close the file, if the fd is found */
	   lock_acquire(&file_lock);
           file_close(cur_file_elem->the_file);
	   lock_release(&file_lock);
           
           /* free the fd_elem and remove from fd_list */
           list_remove(&cur_file_elem->elem);
	   free(cur_file_elem->filename);
           free(cur_file_elem);
	   return 0;
        }
   }
   /* only reaches this code if fd not in list */        
   return -1; // this value doesn't matter
}


/* our own method to check the validity of a user pointer */
bool valid_ptr(const void *ptr)
{
   struct thread *cur = thread_current();
   return (is_user_vaddr(ptr) && pagedir_get_page(cur->pagedir, ptr) != NULL);
}
