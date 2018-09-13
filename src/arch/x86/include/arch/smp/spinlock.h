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

#include <compiler.h>

#if !defined(__PRE_RAM__) \
	|| IS_ENABLED(CONFIG_HAVE_ROMSTAGE_CONSOLE_SPINLOCK)	\
	|| IS_ENABLED(CONFIG_HAVE_ROMSTAGE_NVRAM_CBFS_SPINLOCK)	\
	|| IS_ENABLED(CONFIG_HAVE_ROMSTAGE_MICROCODE_CBFS_SPINLOCK)

/*
 * Your basic SMP spinlocks, allowing only a single CPU anywhere
 */

typedef struct {
	volatile unsigned int lock;
} spinlock_t;

#ifdef __PRE_RAM__
spinlock_t *romstage_console_lock(void);
void initialize_romstage_console_lock(void);
spinlock_t *romstage_nvram_cbfs_lock(void);
void initialize_romstage_nvram_cbfs_lock(void);
spinlock_t *romstage_microcode_cbfs_lock(void);
void initialize_romstage_microcode_cbfs_lock(void);
#endif

#define SPIN_LOCK_UNLOCKED { 1 }

#ifndef __PRE_RAM__
#define DECLARE_SPIN_LOCK(x)	\
	static spinlock_t x = SPIN_LOCK_UNLOCKED;
#else
#define DECLARE_SPIN_LOCK(x)
#endif

/*
 * Simple spin lock operations.  There are two variants, one clears IRQ's
 * on the local processor, one does not.
 *
 * We make no fairness assumptions. They have a cost.
 */
#define barrier() __asm__ __volatile__("" : : : "memory")
#define spin_is_locked(x)	(*(volatile char *)(&(x)->lock) <= 0)
#define spin_unlock_wait(x)	do { barrier(); } while (spin_is_locked(x))

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

static __always_inline void spin_lock(spinlock_t *lock)
{
	__asm__ __volatile__(
		spin_lock_string
		: "=m" (lock->lock) : : "memory");
}

static __always_inline void spin_unlock(spinlock_t *lock)
{
	__asm__ __volatile__(
		spin_unlock_string
		: "=m" (lock->lock) : : "memory");
}

/* REP NOP (PAUSE) is a good thing to insert into busy-wait loops. */
static __always_inline void cpu_relax(void)
{
	__asm__ __volatile__("rep;nop" : : : "memory");
}

#else /* !__PRE_RAM__ */

#define DECLARE_SPIN_LOCK(x)
#define barrier()		do {} while (0)
#define spin_is_locked(lock)	0
#define spin_unlock_wait(lock)	do {} while (0)
#define spin_lock(lock)		do {} while (0)
#define spin_unlock(lock)	do {} while (0)
#define cpu_relax()		do {} while (0)

#endif /* !__PRE_RAM__ */

#endif /* ARCH_SMP_SPINLOCK_H */
