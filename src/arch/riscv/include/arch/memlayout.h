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

#define STACK(addr, size) REGION(stack, addr, size, 4096)

#if defined(__PRE_RAM__)
	#define CAR_STACK(addr, size) \
		REGION(car_stack, addr, size, 4K) \
		ALIAS_REGION(car_stack, stack)
	#define MEM_STACK(addr, size) \
		REGION(mem_stack, addr, size, 4K)
#else
	#define CAR_STACK(addr, size) \
		REGION(car_stack, addr, size, 4K)
	#define MEM_STACK(addr, size) \
		REGION(mem_stack, addr, size, 4K) \
		ALIAS_REGION(mem_stack, stack)
#endif

/* TODO: Need to add DMA_COHERENT region like on ARM? */

#endif /* __ARCH_MEMLAYOUT_H */
