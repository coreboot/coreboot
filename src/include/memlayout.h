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

#ifndef __MEMLAYOUT_H
#define __MEMLAYOUT_H

#include <arch/memlayout.h>

#define STR(x) #x

#define SET_COUNTER(name, addr) \
	_ = ASSERT(. <= addr, STR(name overlaps the previous region!)); \
	. = addr;

#define SYMBOL(name, addr) \
	SET_COUNTER(name, addr) \
	_##name = .;

#define REGION(name, addr, size, expected_align) \
	SYMBOL(name, addr) \
	_ = ASSERT(. == ALIGN(expected_align), \
		STR(name must be aligned to expected_align!)); \
	SYMBOL(e##name, addr + size)

/* Declare according to SRAM/DRAM ranges in SoC hardware-defined address map. */
#define SRAM_START(addr) SYMBOL(sram, addr)

#define SRAM_END(addr) SYMBOL(esram, addr)

#define DRAM_START(addr) SYMBOL(dram, addr)

#define PRERAM_CBMEM_CONSOLE(addr, size) \
	REGION(preram_cbmem_console, addr, size, 4)

/* Use either CBFS_CACHE (unified) or both (PRERAM|POSTRAM)_CBFS_CACHE */
#define CBFS_CACHE(addr, size) REGION(cbfs_cache, addr, size, 4)

/* TODO: This only works if you never access CBFS in romstage before RAM is up!
 * If you need to change that assumption, you have some work ahead of you... */
#if defined(__PRE_RAM__) && !defined(__ROMSTAGE__)
	#define PRERAM_CBFS_CACHE(addr, size) CBFS_CACHE(addr, size)
	#define POSTRAM_CBFS_CACHE(addr, size) \
		REGION(unused_cbfs_cache, addr, size, 4)
#else
	#define PRERAM_CBFS_CACHE(addr, size) \
		REGION(unused_cbfs_cache, addr, size, 4)
	#define POSTRAM_CBFS_CACHE(addr, size) CBFS_CACHE(addr, size)
#endif

/* Careful: 'INCLUDE <filename>' must always be at the end of the output line */
#ifdef __BOOTBLOCK__
	#define BOOTBLOCK(addr, sz) \
		SET_COUNTER(bootblock, addr) \
		_ = ASSERT(_ebootblock - _bootblock <= sz, \
			STR(Bootblock exceeded its allotted size! (sz))); \
		INCLUDE "lib/bootblock.bootblock.ld"
#else
	#define BOOTBLOCK(addr, sz) \
		SET_COUNTER(bootblock, addr) \
		. += sz;
#endif

#ifdef __ROMSTAGE__
	#define ROMSTAGE(addr, sz) \
		SET_COUNTER(romstage, addr) \
		_ = ASSERT(_eromstage - _romstage <= sz, \
			STR(Romstage exceeded its allotted size! (sz))); \
		INCLUDE "lib/romstage.romstage.ld"
#else
	#define ROMSTAGE(addr, sz) \
		SET_COUNTER(romstage, addr) \
		. += sz;
#endif

#ifdef __RAMSTAGE__
	#define RAMSTAGE(addr, sz) \
		SET_COUNTER(ramstage, addr) \
		_ = ASSERT(_eramstage - _ramstage <= sz, \
			STR(Ramstage exceeded its allotted size! (sz))); \
		INCLUDE "lib/ramstage.ramstage.ld"
#else
	#define RAMSTAGE(addr, sz) \
		SET_COUNTER(ramstage, addr) \
		. += sz;
#endif

#endif /* __MEMLAYOUT_H */
