#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#define NUM_SYSCALLS 20

static void syscall_handler (struct intr_frame *);

//syscall_lookup_val handler_table[NUM_SYSCALLS];
syscall_wrapper* handler_table[NUM_SYSCALLS];

void
syscall_init (void) 
{
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

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  uint32_t call_num, arg1, arg2, arg3;
  
  printf ("system call!\n");
  //thread_exit ();
  
  void * esp = f->esp;
  memcpy(&call_num, esp, 4);
  memcpy(&arg1, (esp + 4), 4);
  memcpy(&arg2, (esp + 8), 4);
  memcpy(&arg3, (esp + 12), 4);

  //handler_table[call_num](arg1, arg2, arg3);
}


uint32_t halt_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   
   return 0;
}

uint32_t exit_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
} 
uint32_t exec_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
} 

uint32_t wait_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t create_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t remove_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t open_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t filesize_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t read_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t write_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t seek_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t tell_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}
        
uint32_t close_w(uint32_t arg1 UNUSED, uint32_t arg2 UNUSED, uint32_t arg3 UNUSED)
{
   return 0;
}


