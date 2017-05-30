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
