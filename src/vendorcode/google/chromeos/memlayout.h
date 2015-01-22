/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

/* This file contains macro definitions for memlayout.ld linker scripts. */

#ifndef __CHROMEOS_MEMLAYOUT_H
#define __CHROMEOS_MEMLAYOUT_H

/* Careful: required work buffer size depends on RW properties such as key size
 * and algorithm -- what works for you might stop working after an update. Do
 * NOT lower the asserted minimum without consulting vboot devs (rspangler)! */
#define VBOOT2_WORK(addr, size) \
	REGION(vboot2_work, addr, size, 8) \
	_ = ASSERT(size >= 12K, "vboot2 work buffer must be at least 12K!");

#ifdef __VERSTAGE__
	#define VERSTAGE(addr, sz) \
		SET_COUNTER(VERSTAGE, addr) \
		_ = ASSERT(_everstage - _verstage <= sz, \
			STR(Verstage exceeded its allotted size! (sz))); \
		INCLUDE "vendorcode/google/chromeos/vboot2/verstage.verstage.ld"
#else
	#define VERSTAGE(addr, sz) \
		SET_COUNTER(VERSTAGE, addr) \
		. += sz;
#endif

#ifdef __VERSTAGE__
	#define OVERLAP_VERSTAGE_ROMSTAGE(addr, size) VERSTAGE(addr, size)
#else
	#define OVERLAP_VERSTAGE_ROMSTAGE(addr, size) ROMSTAGE(addr, size)
#endif

#define WATCHDOG_TOMBSTONE(addr, size) \
	REGION(watchdog_tombstone, addr, size, 4) \
	_ = ASSERT(size == 4, "watchdog tombstones should be exactly 4 byte!");

#endif /* __CHROMEOS_MEMLAYOUT_H */
