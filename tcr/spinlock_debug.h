#ifndef SPINLOCK_DEBUG_H
#define SPINLOCK_DEBUG_H

#include <sched.h>
#include "atomic.h"

typedef struct {
	int lock;
	char* holder;
	char* file;
	int line;
} spinlock_t;

#include "coroutines.h"
#define spin_lock(LOCK)  __spin_lock(LOCK, *((char **)cr_uptr(cr_current())), __FILE__, __LINE__)

void msg_exit(int code, const char *fmt, ...) __attribute__ ((__noreturn__));
static inline void __spin_lock(spinlock_t *l, char* holder, char* file, int line)
{
	int i = 0;
	while (!__sync_bool_compare_and_swap(&l->lock, 0, 1)) {
		i++;
		if ((i & ((1<<12)-1)) == 0) /* every 4096 spins, call sched_yield() */
			sched_yield();

		if ((i>>22) & 1) {/* eventually abort the program. */
			fprintf(stderr, "lock held by: \"%s\" in %s:%d\n",
				l->holder, l->file, l->line);
			fprintf(stderr, "\"%s\" tries to get lock in %s:%d\n",
				holder, file, line);
			msg_exit(1, "spinning too long in spin_lock()\n");

		}
	}
	l->file = file;
	l->line = line;
	l->holder = holder;
}

static inline void spin_unlock(spinlock_t *l)
{
	__sync_lock_release(&l->lock);
	l->file = "(none)";
	l->holder = "(none)";
	l->line = 0;
}

#endif