#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"


tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

/* our helper function(s) */
inline int size_of_char_arg(char *arg);
inline void* word_align(void* add, int size);

/* for file synchronization, used in process.c and syscall.c */
struct lock file_lock;

#endif /* userprog/process.h */
