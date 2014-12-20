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
 * Foundation, Inc.
 */

/* This file contains macro definitions for memlayout.ld linker scripts. */

#ifndef __ARCH_MEMLAYOUT_H
#define __ARCH_MEMLAYOUT_H

#define SUPERPAGE_SIZE ((1 + IS_ENABLED(CONFIG_ARM_LPAE)) * 1M)

#define TTB(addr, size) \
	REGION(ttb, addr, size, 16K) \
	_ = ASSERT(size >= 16K + IS_ENABLED(CONFIG_ARM_LPAE) * 32, \
		"TTB must be 16K (+ 32 for LPAE)!");

#define TTB_SUBTABLES(addr, size) \
	REGION(ttb_subtables, addr, size, IS_ENABLED(CONFIG_ARM_LPAE)*3K + 1K) \
	_ = ASSERT(size % (1K + 3K * IS_ENABLED(CONFIG_ARM_LPAE)) == 0, \
		"TTB subtable region must be evenly divisible by table size!");

/* ARM stacks need 8-byte alignment and stay in one place through ramstage. */
#define STACK(addr, size) \
	REGION(stack, addr, size, 8) \
	_ = ASSERT(size >= 2K, "stack should be >= 2K, see toolchain.inc");

#define DMA_COHERENT(addr, size) \
	REGION(dma_coherent, addr, size, SUPERPAGE_SIZE) \
	_ = ASSERT(size % SUPERPAGE_SIZE == 0, \
		"DMA coherency buffer must fit exactly in full superpages!");

#define FRAMEBUFFER(addr, size) \
	REGION(framebuffer, addr, size, SUPERPAGE_SIZE) \
	_ = ASSERT(size % SUPERPAGE_SIZE == 0, \
		"Framebuffer must fit exactly in full superpages!");

#endif /* __ARCH_MEMLAYOUT_H */
