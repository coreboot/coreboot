/* SPDX-License-Identifier: GPL-2.0-only */

/* This file contains macro definitions for memlayout.ld linker scripts. */

#ifndef __ARCH_MEMLAYOUT_H
#define __ARCH_MEMLAYOUT_H

#define STACK(addr, size) REGION(stack, addr, size, 4096)

#if ENV_ROMSTAGE_OR_BEFORE
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
