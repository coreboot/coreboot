/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SMP_SPINLOCK_H
#define SMP_SPINLOCK_H

#if ENV_SUPPORTS_SMP
#include <arch/smp/spinlock.h>
#else /* !CONFIG_SMP */

#define DECLARE_SPIN_LOCK(x)
#define spin_is_locked(lock)	0
#define spin_unlock_wait(lock)	do {} while (0)
#define spin_lock(lock)		do {} while (0)
#define spin_unlock(lock)	do {} while (0)
#endif

#endif /* SMP_SPINLOCK_H */
