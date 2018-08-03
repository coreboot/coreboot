/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Imagination Technologies
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

#ifndef __MIPS_ARCH_CACHE_H
#define __MIPS_ARCH_CACHE_H

#include <stddef.h>
#include <stdint.h>

#define CACHE_TYPE_SHIFT	(0)
#define CACHE_OP_SHIFT		(2)
#define CACHE_TYPE_MASK		(0x3)
#define CACHE_OP_MASK		(0x7)

/* Cache type */
#define ICACHE			0x00
#define DCACHE			0x01
#define L2CACHE			0x03

/* Cache operation*/
#define WB_INVD			0x05

#define CACHE_CODE(type, op)	((((type) & (CACHE_TYPE_MASK)) <<	\
				(CACHE_TYPE_SHIFT)) |			\
				(((op) & (CACHE_OP_MASK)) << (CACHE_OP_SHIFT)))

/* Perform cache operation on cache lines for target addresses */
void perform_cache_operation(uintptr_t start, size_t size, uint8_t operation);
/* Invalidate all caches: instruction, data, L2 data */
void cache_invalidate_all(uintptr_t start, size_t size);

/* TODO: Global cache API. Implement properly once we finally have a MIPS board
   again where we can figure out what exactly these should be doing. */
static inline void dcache_clean_all(void) {}
static inline void dcache_invalidate_all(void) {}
static inline void dcache_clean_invalidate_all(void) {}

#endif /* __MIPS_ARCH_CACHE_H */
