/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

/*
 * The aim of this code is to bring the machine from power-on to the point 
 * where we can jump to the the main LinuxBIOS entry point hardwaremain()
 * which is written in C.
 *
 * At power-on, we have no RAM, a memory-mapped I/O space, and we are executing
 * out of ROM, generally at 0xfff00100.
 *
 * Before we jump to harwaremain() we want to do the following:
 *
 * - enable L1 I/D caches, otherwise performance will be slow
 * - set up DBATs for the following regions:
 *   - RAM (generally 0x00000000 - 0x7fffffff)
 *   - ROM (_ROMBASE - _ROMBASE + 16Mb)
 *   - I/O (generally 0xfc000000 - 0xfdffffff)
 *   - the main purpose for setting up the DBATs is so the I/O region
 *     can be marked cache inhibited/write through
 * - set up IBATs for RAM and ROM
 *
 */

#define BSP_IOREGION1	0x80000000
#define BSP_IOMASK1	BAT_BL_256M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
#define BSP_IOREGION2	0xFD000000
#define BSP_IOMASK2	BAT_BL_64M | BAT_VALID_SUPERVISOR | BAT_VALID_USER

        isync

	/* 
	 * Disable dcache and MMU, so we're in a known state
	 */   
        li      r0, 0
	sync
        mtspr	HID0, r0
        sync
        mtmsr	r0 
        isync

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
	 * Clear segment registers (LinuxBIOS doesn't use these)
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
	 * Initialize northbridge. This has to happen early because it
	 * resets memory. Memory is on at this point, albeit with
	 * pessimistic settings. We reconfigure later using I2C.
	 */
	bl	bsp_init_northbridge

        /*
	 * Set up DBATs 
	 *
	 * DBAT0 covers RAM (0 - 256Mb)
	 * DBAT1 covers PCI memory and ROM (0xFC000000 - 0xFFFFFFFF)
	 * DBAT1 covers PCI memory (0x80000000 - 0x8FFFFFFF)
	 */
        lis     r2, 0@ha
        ori     r3, r2, BAT_BL_256M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
        ori     r2, r2, BAT_READ_WRITE
        mtdbatu 0, r3
        mtdbatl 0, r2
	isync

        lis     r2, BSP_IOREGION2@ha
        ori     r3, r2, BAT_BL_64M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
        ori     r2, r2, BAT_CACHE_INHIBITED | BAT_GUARDED | BAT_READ_WRITE
        mtdbatu 1, r3
        mtdbatl 1, r2
	isync

        lis     r2, BSP_IOREGION1@h
        ori     r3, r2, BSP_IOMASK1
        ori     r2, r2, BAT_CACHE_INHIBITED | BAT_GUARDED | BAT_READ_WRITE
        mtdbatu 2, r3
        mtdbatl 2, r2
	isync

	/*
	 * IBATS
	 *
	 * IBAT0 covers RAM (0 - 256Mb)
	 * IBAT1 covers ROM (_ROMBASE - _ROMBASE+16M)
	 */
        lis     r2, 0@ha
        ori     r3, r2, BAT_BL_256M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
        ori     r2, r2, BAT_READ_WRITE
        mtibatu 0, r3
        mtibatl 0, r2
	isync

        lis     r2, _ROMBASE@ha
        ori     r3, r2, BAT_BL_16M | BAT_VALID_SUPERVISOR | BAT_VALID_USER
        ori     r2, r2, BAT_READ_ONLY
        mtibatu 1, r3
        mtibatl 1, r2
	isync

	/*
	 * Invalidate tlb entries
	 */
	lis	r3, 0
	lis	r5, 0x4
	isync
tlblp:
	tlbie	r3
	sync
	addi	r3, r3, 0x1000
	cmp	0, 0, r3, r5
	blt	tlblp

	sync

        /*
	 * Enable MMU 
	 */
        mfmsr   r2
        ori     r2, r2, MSR_DR | MSR_IR
	isync
        mtmsr   r2
        isync

        /*
	 * Enable L1 dcache 
	 */
        mfspr   r2, HID0
        ori     r2, r2, HID0_DCE | HID0_DCFI
	sync
        mtspr   HID0, r2

        /*
	 * Enable L1 icache 
	 */
        mfspr   r2, HID0
        ori     r2, r2, HID0_ICE | HID0_ICFI
	isync
        mtspr   HID0, r2

	/*
	 * Must branch to start_payload once CPU initialization is completed
	 */
	b	start_payload
