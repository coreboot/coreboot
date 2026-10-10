/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ARCH_SMP_SPINLOCK_H
#define ARCH_SMP_SPINLOCK_H

#include <arch/barrier.h>
#include <arch/smp/atomic.h>

typedef struct {
	atomic_t lock;
} spinlock_t;

#define SPIN_LOCK_UNLOCKED { { 0 } }

#define DECLARE_SPIN_LOCK(x)	\
	static spinlock_t x = SPIN_LOCK_UNLOCKED;

#define spin_is_locked(x)	(atomic_read(&(x)->lock) != 0)
#define spin_unlock_wait(x)	do { } while (spin_is_locked(x))

/* Only a real lock across harts if atomic_swap() is atomic (A extension) */
static inline void spin_lock(spinlock_t *lock)
{
	while (atomic_swap(&lock->lock, -1))
		;
	barrier();
}

static inline void spin_unlock(spinlock_t *lock)
{
	barrier();
	atomic_set(&lock->lock, 0);
}

#endif // ARCH_SMP_SPINLOCK_H
