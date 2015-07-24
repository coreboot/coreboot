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

/* MIPS stacks need 8-byte alignment and stay in one place through ramstage. */
/* TODO: Double-check that that's the correct alignment for our ABI. */
#define STACK(addr, size) \
	REGION(stack, addr, size, 8) \
	_ = ASSERT(size >= 2K, "stack should be >= 2K, see toolchain.inc");

#define DMA_COHERENT(addr, size) REGION(dma_coherent, addr, size, 4K)

#define SOC_REGISTERS(addr, size) REGION(soc_registers, addr, size, 4)

#endif /* __ARCH_MEMLAYOUT_H */
