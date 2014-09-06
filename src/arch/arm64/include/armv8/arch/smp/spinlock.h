#ifndef ARCH_SMP_SPINLOCK_H
#define ARCH_SMP_SPINLOCK_H

#include <arch/barrier.h>
#include <stdint.h>

typedef struct {
	volatile uint32_t lock;
} spinlock_t;

#define SPIN_LOCK_UNLOCKED (spinlock_t) { 0 }
#define DECLARE_SPIN_LOCK(x) static spinlock_t x = SPIN_LOCK_UNLOCKED;

static inline void spin_lock(spinlock_t *spin)
{
	while (1) {
		if (load_acquire_exclusive(&spin->lock) != 0)
			continue;
		if (store_release_exclusive(&spin->lock, 1))
			break;
	}
}

static inline void spin_unlock(spinlock_t *spin)
{
	store_release(&spin->lock, 0);
}

#endif
