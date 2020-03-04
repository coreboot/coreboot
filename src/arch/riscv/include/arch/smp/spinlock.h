/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef ARCH_SMP_SPINLOCK_H
#define ARCH_SMP_SPINLOCK_H

#include <arch/encoding.h>
#include <arch/smp/atomic.h>

#define barrier() { asm volatile ("fence" ::: "memory"); }

typedef struct {
	atomic_t lock;
} spinlock_t;

static inline void spinlock_lock(spinlock_t *lock)
{
	while (atomic_swap(&lock->lock, -1))
		;
	barrier();
}

static inline void spinlock_unlock(spinlock_t *lock)
{
	barrier();
	atomic_set(&lock->lock, 0);
}

#endif // ARCH_SMP_SPINLOCK_H
