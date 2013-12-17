#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <inttypes.h>

void syscall_init (void);

/* All system calls have the same type. They take exactly 3 arguments, and return
 * a value, whether the system call needs to return anything, or not.
 * NOTE: The name is a holdover from a design decision that fell through. */
typedef uint32_t syscall_wrapper(uint32_t arg1, uint32_t arg2, uint32_t arg3);

/* Declaration for all of are system calls. 
 * NOTE: the w is a holdover and doesn't mean anything. */
syscall_wrapper halt_w, exit_w, exec_w, wait_w,
        create_w, remove_w, open_w, filesize_w,
        read_w, write_w, seek_w, tell_w, close_w;
        /* NOTE: must include other calls for later projects */
        /* code in syscall will like break if they are called */

#endif /* userprog/syscall.h */
