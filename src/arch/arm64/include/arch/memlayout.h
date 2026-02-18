/* SPDX-License-Identifier: GPL-2.0-only */

/* This file contains macro definitions for memlayout.ld linker scripts. */

#ifndef __ARCH_MEMLAYOUT_H
#define __ARCH_MEMLAYOUT_H

#define TTB(addr, size) \
	REGION(ttb, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, "TTB size must be divisible by 4K!");

#if ENV_ROMSTAGE_OR_BEFORE
	#define PRERAM_TTB(addr, size) \
		REGION(preram_ttb, addr, size, 4K) \
		_ = ASSERT(size % 4K == 0, "preram_ttb size must be divisible by 4K!"); \
		ALIAS_REGION(preram_ttb, ttb)
	#define POSTRAM_TTB(addr, size) \
		REGION(postram_ttb, addr, size, 4K) \
		_ = ASSERT(size >= _preram_ttb_size, "The postram_ttb size cannot be smaller than the preram_ttb size.");
#else
	#define PRERAM_TTB(addr, size) \
		REGION(preram_ttb, addr, size, 4K)
	#define POSTRAM_TTB(addr, size) \
		REGION(postram_ttb, addr, size, 4K) \
		_ = ASSERT(size % 4K == 0, "postram_ttb size must be divisible by 4K!"); \
		_ = ASSERT(size >= _preram_ttb_size, "The postram_ttb size cannot be smaller than the preram_ttb size."); \
		ALIAS_REGION(postram_ttb, ttb)
#endif

#define FRAMEBUFFER(addr, size) \
	REGION(framebuffer, addr, size, 1M) \
	_ = ASSERT(size % 1M == 0, \
		"Framebuffer must fit exactly in 1M!");

/* ARM64 stacks need 16-byte alignment. */
#define STACK(addr, size) \
	REGION(stack, addr, size, 16) \
	_ = ASSERT(size >= 2K, "stack should be >= 2K, see toolchain.mk"); \
	ALIAS_REGION(stack, preram_stack) \
	ALIAS_REGION(stack, postram_stack)

#if ENV_ROMSTAGE_OR_BEFORE
	#define PRERAM_STACK(addr, size) \
		REGION(preram_stack, addr, size, 16) \
		_ = ASSERT(size >= 2K, "preram_stack should be >= 2K, see toolchain.mk"); \
		ALIAS_REGION(preram_stack, stack)
	#define POSTRAM_STACK(addr, size) \
		REGION(postram_stack, addr, size, 16)
#else
	#define PRERAM_STACK(addr, size) \
		REGION(preram_stack, addr, size, 16)
	#define POSTRAM_STACK(addr, size) \
		REGION(postram_stack, addr, size, 16) \
		_ = ASSERT(size >= 2K, "postram_stack should be >= 2K, see toolchain.mk"); \
		ALIAS_REGION(postram_stack, stack)
#endif

#define BL31(addr, size) \
	REGION(bl31, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, "BL31 size must be divisible by 4K!");

#endif /* __ARCH_MEMLAYOUT_H */
