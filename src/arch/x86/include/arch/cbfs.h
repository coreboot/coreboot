/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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

#ifndef __INCLUDE_ARCH_CBFS__
#define __INCLUDE_ARCH_CBFS__

static void *walkcbfs(char *target)
{
	void *entry;
	asm volatile (
		"mov $1f, %%esp\n\t"
		"jmp walkcbfs_asm\n\t"
		"1:\n\t" : "=a" (entry) : "S" (target) : "ebx", "ecx", "edi", "esp");
	return entry;
}

/* just enough to support findstage. copied because the original version doesn't easily pass through romcc */
struct cbfs_stage_restricted {
	unsigned long compression;
	unsigned long entry; // this is really 64bit, but properly endianized
};

static inline unsigned long findstage(char* target)
{
	return ((struct cbfs_stage_restricted *)walkcbfs(target))->entry;
}

static inline void call(unsigned long addr, unsigned long bist)
{
	asm volatile ("jmp *%0\n\t" : : "r" (addr), "a" (bist));
}
#endif
