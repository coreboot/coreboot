/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _ARCH_SMP_SPINLOCK_H
#define _ARCH_SMP_SPINLOCK_H

#define DECLARE_SPIN_LOCK(x)
#define barrier()		do {} while (0)
#define spin_is_locked(lock)	0
#define spin_unlock_wait(lock)	do {} while (0)
#define spin_lock(lock)		do {} while (0)
#define spin_unlock(lock)	do {} while (0)
#define cpu_relax()		do {} while (0)

#include <smp/node.h>
#define boot_cpu() 1

#endif
