/*
 * This file is part of the LinuxBIOS project.
 *
 * It is based on the Linux kernel file include/asm-i386/spinlock.h.
 *
 * Modifications are:
 * Copyright (C) 2001 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
 */

#ifndef ARCH_SPINLOCK_H
#define ARCH_SPINLOCK_H

/*
 * Your basic SMP spinlocks, allowing only a single CPU anywhere.
 */

struct spinlock {
	volatile unsigned int lock;
};

#define SPIN_LOCK_UNLOCKED (struct spinlock) { 1 }

/*
 * Simple spin lock operations.  There are two variants, one clears IRQ's
 * on the local processor, one does not.
 *
 * We make no fairness assumptions. They have a cost.
 */
#define barrier()		__asm__ __volatile__("": : :"memory")
#define spin_is_locked(x)	(*(volatile char *)(&(x)->lock) <= 0)
#define spin_unlock_wait(x)	do { barrier(); } while(spin_is_locked(x))

#define spin_lock_string \
	"\n1:\t" \
	"lock ; decb %0\n\t" \
	"js 2f\n" \
	".section .text.lock,\"ax\"\n" \
	"2:\t" \
	"cmpb $0,%0\n\t" \
	"rep;nop\n\t" \
	"jle 2b\n\t" \
	"jmp 1b\n" \
	".previous"

/*
 * This works. Despite all the confusion.
 */
#define spin_unlock_string \
	"movb $1,%0"

static inline __attribute__((always_inline)) void spin_lock(struct spinlock *lock)
{
	__asm__ __volatile__(
		spin_lock_string
		:"=m" (lock->lock) : : "memory");
}

static inline __attribute__((always_inline)) void spin_unlock(struct spinlock *lock)
{
	__asm__ __volatile__(
		spin_unlock_string
		:"=m" (lock->lock) : : "memory");
}

#define spin_define(spin) static struct spinlock spin = SPIN_LOCK_UNLOCKED

#endif /* ARCH_SPINLOCK_H */
