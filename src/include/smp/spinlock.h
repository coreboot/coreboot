#ifndef SMP_SPINLOCK_H
#define SMP_SPINLOCK_H

#if CONFIG_SMP == 1
#include <arch/smp/spinlock.h>
#else /* !CONFIG_SMP */

/* Most GCC versions have a nasty bug with empty initializers */
#if (__GNUC__ > 2) 
typedef struct { } spinlock_t;
#define SPIN_LOCK_UNLOCKED (spinlock_t) { }
#else
typedef struct { int gcc_is_buggy; } spinlock_t;
#define SPIN_LOCK_UNLOCKED (spinlock_t) { 0 }
#endif

#define barrier()		do {} while(0)
#define spin_is_locked(lock)	0
#define spin_unlock_wait(lock)	do {} while(0)
#define spin_lock(lock)		do {} while(0)
#define spin_unlock(lock)	do {} while(0)
#define cpu_relax()		do {} while(0)
#endif

#endif /* SMP_SPINLOCK_H */
