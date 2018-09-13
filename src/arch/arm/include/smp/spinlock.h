/*
 * This file is part of the coreboot project.
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

/* FIXME: implement this for ARM */
#error "implement this for ARM"
#if 0
/*
 * Your basic SMP spinlocks, allowing only a single CPU anywhere
 */

typedef struct {
	volatile unsigned int lock;
} spinlock_t;


#define SPIN_LOCK_UNLOCKED (spinlock_t) { 1 }
#define DECLARE_SPIN_LOCK(x) static spinlock_t x = SPIN_LOCK_UNLOCKED;

#define barrier() (__asm__ __volatile__("" : : : "memory"))
#define spin_is_locked(x)	(*(volatile char *)(&(x)->lock) != 0)
#define spin_unlock_wait(x)	do { barrier(); } while (spin_is_locked(x))

static __always_inline void spin_lock(spinlock_t *lock)
{
	unsigned long tmp;
	__asm__ __volatile__ (
		"1:	ldrex	%0, [%1]\n"
		"	teq	%0, #0\n"
		"	strexeq	%0, %2, [%1]\n"
		"	teqeq	%0, #0\n"
		"	bne	1b\n"
		: "=&r" (tmp)
		: "r" (&lock->lock), "r" (1)
		: "cc"
	);
	barrier();
}

static __always_inline void spin_unlock(spinlock_t *lock)
{
	__asm__ __volatile__(
		"	str	%1, [%0]\n"
		:
		: "r" (&lock->lock), "r" (0)
		: "cc"
	);
}

#define cpu_relax() barrier()

#endif
#endif /* ARCH_SMP_SPINLOCK_H */
