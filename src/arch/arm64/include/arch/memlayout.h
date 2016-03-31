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

#endif /* __ARCH_MEMLAYOUT_H */
