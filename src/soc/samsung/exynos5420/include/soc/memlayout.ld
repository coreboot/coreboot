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
 */

#include <memlayout.h>

#include <arch/header.ld>

/*
 * Note: The BootROM loads the 8K BL1 at [0x2020000:0x2022000), so the bootblock
 * must be placed after that. After the handoff, the space can be reclaimed.
 */

SECTIONS
{
	SRAM_START(0x2020000)
	/* 17K hole, includes BL1 */
	/* Bootblock is preceded by 16 byte variable length BL2 checksum. */
	BOOTBLOCK(0x2024410, 32K - 16)
	/* 15K hole */
	ROMSTAGE(0x2030000, 128K)
	/* 32K hole */
	TTB(0x2058000, 16K)
	PRERAM_CBFS_CACHE(0x205C000, 76K)
	STACK(0x206F000, 16K)
	/* 1K hole for weird kernel-shared CPU/SMP state structure that doesn't
	 * seem to be implemented right now? */
	SRAM_END(0x2074000)

	DRAM_START(0x20000000)
	RAMSTAGE(0x20000000, 128K)
	POSTRAM_CBFS_CACHE(0x21000000, 8M)
	DMA_COHERENT(0x77300000, 1M)
}
