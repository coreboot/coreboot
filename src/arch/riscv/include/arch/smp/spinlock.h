/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 HardenedLinux.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef ARCH_SMP_SPINLOCK_H
#define ARCH_SMP_SPINLOCK_H

#include <arch/encoding.h>
#include <arch/smp/atomic.h>

#define barrier() { asm volatile ("fence" ::: "memory"); }

typedef struct {
	volatile atomic_t lock;
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
