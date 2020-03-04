/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* This file contains macro definitions for memlayout.ld linker scripts. */

#ifndef __ARCH_MEMLAYOUT_H
#define __ARCH_MEMLAYOUT_H

#define TTB(addr, size) \
	REGION(ttb, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, "TTB size must be divisible by 4K!");

#define DMA_COHERENT(addr, size) \
	REGION(dma_coherent, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, \
		"DMA buffer should be multiple of smallest page size (4K)!");

#define FRAMEBUFFER(addr, size) \
	REGION(framebuffer, addr, size, 1M) \
	_ = ASSERT(size % 1M == 0, \
		"Framebuffer must fit exactly in 1M!");

/* ARM64 stacks need 16-byte alignment. */
#define STACK(addr, size) \
	REGION(stack, addr, size, 16) \
	_ = ASSERT(size >= 2K, "stack should be >= 2K, see toolchain.inc");

#define BL31(addr, size) \
	REGION(bl31, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, "BL31 size must be divisible by 4K!");

#endif /* __ARCH_MEMLAYOUT_H */
