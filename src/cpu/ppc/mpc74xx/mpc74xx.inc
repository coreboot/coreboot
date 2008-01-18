/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/*
 * The aim of this code is to bring the machine from power-on to the point 
 * where we can jump to the the main coreboot entry point hardwaremain()
 * which is written in C.
 *
 * At power-on, we have no RAM, a memory-mapped I/O space, and we are executing
 * out of ROM, generally at 0xfff00100.
 *
 * Before we jump to harwaremain() we want to do the following:
 *
 * - enable L1 I/D caches, otherwise performance will be slow
 * - set up DBATs for the following regions:
 *   - RAM (generally 0x00000000 -> 0x7fffffff)
 *   - ROM (_ROMBASE -> _ROMBASE + ROM_SIZE)
 *   - I/O (generally 0xfc000000 -> 0xfdffffff)
 *   - the main purpose for setting up the DBATs is so the I/O region
 *     can be marked cache inhibited/write through
 * - set up IBATs for RAM and ROM
 *
 */

#include <ppc74xx.h>

#define BSP_IOREGION1	0x80000000
#define BSP_IOMASK1	BAT_BL_256M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
#define BSP_IOREGION2	0xFD000000
#define BSP_IOMASK2	BAT_BL_64M | BAT_VALID_SUPERVISOR | BAT_VALID_USER

        isync
	sync

	/* 
	 * Disable dcache and MMU
	 * Clear IR & DR
	 * Enable FP
	 */   
        li      r0, 0
        mtspr	HID0, r0
	sync
	mfmsr	r3
	li	r4, 0x0030	/* IR & DR */
	andc	r3, r3, r4
	ori	r3, r3, 0x2000	/* FP */
	mtmsr	r3
        isync
        sync

	/*
	 * Invalidate D & I BATS
	 */
        mtibatu 0, r0
        mtibatu 1, r0
        mtibatu 2, r0
        mtibatu 3, r0
	isync
        mtdbatu 0, r0
        mtdbatu 1, r0
        mtdbatu 2, r0
        mtdbatu 3, r0
	isync

	/*
	 * Clear segment registers (coreboot doesn't use these)
	 */
	mtsr    0, r0
	isync
	mtsr    1, r0
	isync
	mtsr    2, r0
	isync
	mtsr    3, r0
	isync
	mtsr    4, r0
	isync
	mtsr    5, r0
	isync
	mtsr    6, r0
	isync
	mtsr    7, r0
	isync
	mtsr    8, r0
	isync
	mtsr    9, r0
	isync
	mtsr    10, r0
	isync
	mtsr    11, r0
	isync
	mtsr    12, r0
	isync
	mtsr    13, r0
	isync
	mtsr    14, r0
	isync
	mtsr    15, r0
	isync

        /*
	 * Set up DBATs 
	 *
	 * DBAT0 covers RAM (0 -> 0x0FFFFFFF) (256Mb)
	 * DBAT1 covers PCI memory and ROM (0xFD000000 -> 0xFFFFFFFF) (64Mb)
	 * DBAT2 covers PCI memory (0x80000000 -> 0x8FFFFFFF) (256Mb)
	 * DBAT3 is not used
	 */
        lis     r2, 0@h
        ori     r3, r2, BAT_BL_256M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
        ori     r2, r2, BAT_READ_WRITE | BAT_GUARDED
        mtdbatu 0, r3
        mtdbatl 0, r2
	isync

        lis     r2, BSP_IOREGION2@h
        ori     r3, r2, BAT_BL_64M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
        ori     r2, r2, BAT_CACHE_INHIBITED | BAT_GUARDED | BAT_READ_WRITE
        mtdbatu 1, r3
        mtdbatl 1, r2
	isync

        lis     r2, BSP_IOREGION1@h
        ori     r3, r2, BAT_BL_256M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
        ori     r2, r2, BAT_CACHE_INHIBITED | BAT_GUARDED | BAT_READ_WRITE
        mtdbatu 2, r3
        mtdbatl 2, r2
	isync

	/*
	 * IBATS
	 *
	 * IBAT0 covers RAM (0 -> 256Mb)
	 * IBAT1 covers ROM (_ROMBASE -> _ROMBASE+ROM_SIZE)
	 */
        lis     r2, 0@h
        ori     r3, r2, BAT_BL_256M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
        ori     r2, r2, BAT_READ_WRITE
        mtibatu 0, r3
        mtibatl 0, r2
	isync

        lis     r2, _ROMBASE@h
#if ROM_SIZE > 1048576
        ori     r3, r2, BAT_BL_16M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
#else
        ori     r3, r2, BAT_BL_1M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
#endif
        ori     r2, r2, BAT_READ_ONLY
        mtibatu 1, r3
        mtibatl 1, r2
	isync

        /*
	 * Enable MMU 
	 */
        mfmsr   r2
        ori     r2, r2, MSR_DR | MSR_IR
        mtmsr   r2
        isync
	sync

        /*
	 * Enable and invalidate the L1 icache 
	 */
        mfspr   r2, HID0
        ori     r2, r2, HID0_ICE | HID0_ICFI
	isync
        mtspr   HID0, r2
        /*
	 * Enable and invalidate the L1 dcache 
	 */
        mfspr   r2, HID0
        ori     r2, r2, HID0_DCE | HID0_DCFI
	sync
        mtspr   HID0, r2
