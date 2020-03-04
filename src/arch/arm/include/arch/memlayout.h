/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* This file contains macro definitions for memlayout.ld linker scripts. */

#ifndef __ARCH_MEMLAYOUT_H
#define __ARCH_MEMLAYOUT_H

#define SUPERPAGE_SIZE ((1 + CONFIG(ARM_LPAE)) * 1M)

#define TTB(addr, size) \
	REGION(ttb, addr, size, 16K) \
	_ = ASSERT(size >= 16K + CONFIG(ARM_LPAE) * 32, \
		"TTB must be 16K (+ 32 for LPAE)!");

#define TTB_SUBTABLES(addr, size) \
	REGION(ttb_subtables, addr, size, CONFIG(ARM_LPAE)*3K + 1K) \
	_ = ASSERT(size % (1K + 3K * CONFIG(ARM_LPAE)) == 0, \
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
