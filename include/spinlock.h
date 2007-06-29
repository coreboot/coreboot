/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2001 Linux Networx
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 	
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SPINLOCK_H
#define SPINLOCK_H

#if defined(CONFIG_SMP) && CONFIG_SMP == 1
#include <arch/spinlock.h>
#else /* !CONFIG_SMP */

/* Most GCC versions have a nasty bug with empty initializers */
#if (__GNUC__ > 2) 
typedef struct { } spinlock_t;
#define SPIN_LOCK_UNLOCKED (spinlock_t) { }
#else
typedef struct { int gcc_is_buggy; } spinlock_t;
#define SPIN_LOCK_UNLOCKED (spinlock_t) { 0 }
#endif

#define barrier()		do {} while(0)
#define spin_is_locked(lock)	0
#define spin_unlock_wait(lock)	do {} while(0)
#define spin_lock(lock)		do {} while(0)
#define spin_unlock(lock)	do {} while(0)
#define cpu_relax()		do {} while(0)
#endif

#endif /* SPINLOCK_H */
