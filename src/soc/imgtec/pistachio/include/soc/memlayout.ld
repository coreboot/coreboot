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

/* SRAM memory is mapped in two different locations. Define regions in both for
 * full overlap checking and use this to guarantee they're kept in sync. */
#define ASSERT_MIRRORED(r1, r2) \
	_ = ASSERT((_e##r1 - _##r1) == (_e##r2 - _##r2) && \
		   (_##r1 & 0x7fffffff) == (_##r2 & 0x7fffffff), \
		   STR(r1 and r2 do not match!));

SECTIONS
{
	/*
	 * All of DRAM (other than the DMA coherent area) is accessed through
	 * the identity mapping.
	 */
	DRAM_START(0x00000000)
	/* DMA coherent area: accessed via KSEG1. */
	DMA_COHERENT(0x00100000, 1M)
	POSTRAM_CBFS_CACHE(0x00200000, 512K)
	RAMSTAGE(0x00280000, 128K)

	/* 0x18100000 -> 0x18540000 */
	SOC_REGISTERS(0x18100000, 0x440000)
	/*
	 * GRAM becomes the SRAM.  Accessed through KSEG0 in the bootblock
	 * and then through the identity mapping in ROM stage.
	 */
	SRAM_START(0x1a000000)
	REGION(gram_bootblock, 0x1a000000, 28K, 1)
	ROMSTAGE(0x1a007000, 60K)
	VBOOT2_WORK(0x1a016000, 12K)
	VBOOT2_TPM_LOG(0x1a019000, 2K)
	PRERAM_CBFS_CACHE(0x1a019800, 46K)
	SRAM_END(0x1a066000)

	/* Bootblock executes out of KSEG0 and sets up the identity mapping.
	 * This is identical to SRAM above, and thus also limited 64K and
	 * needs to avoid conflicts with items set up above.
	 */
	BOOTBLOCK(0x9a000000, 28K)
	REGION(kseg0_romstage, 0x9a007000, 60K, 1)

	/*
	 * Let's use SRAM for stack and CBMEM console.  Always accessed
	 * through KSEG0.
	 */
	STACK(0x9b000000, 8K)
	PRERAM_CBMEM_CONSOLE(0x9b002000, 8K)

}

ASSERT_MIRRORED(bootblock, gram_bootblock)
ASSERT_MIRRORED(romstage, kseg0_romstage)
