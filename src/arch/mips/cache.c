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

#include <arch/cache.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <program_loading.h>
#include <symbols.h>

/* cache_op: issues cache operation for specified address */
#define cache_op(op, addr)						\
({									\
	__asm__ __volatile__(						\
		".set push\n\t"						\
		".set noreorder\n\t"					\
		".set mips32\n\t"					\
		"cache %0, %1\n\t"					\
		".set mips0\n\t"					\
		".set pop\n\t"						\
		:							\
		: "i" (op), "R" (*(unsigned char *)(addr)));		\
})

#define MIPS_CONFIG1_DL_SHIFT	10
#define MIPS_CONFIG1_DL_MASK	(0x00000007)
#define MIPS_CONFIG1_IL_SHIFT	19
#define MIPS_CONFIG1_IL_MASK	(0x00000007)
#define MIPS_CONFIG2_SL_SHIFT	4
#define MIPS_CONFIG2_SL_MASK	(0x0000000F)

/*
 * get_cache_line_size:
 * Read config register
 * Isolate instruction cache line size
 * Interpret value as per MIPS manual: 2 << value
 * Return cache line size
 */
static int get_cache_line_size(uint8_t type)
{
	switch (type) {
	case ICACHE:
		return 2 << ((read_c0_config1() >> MIPS_CONFIG1_IL_SHIFT) &
				MIPS_CONFIG1_IL_MASK);
	case DCACHE:
		return 2 << ((read_c0_config1() >> MIPS_CONFIG1_DL_SHIFT) &
				MIPS_CONFIG1_DL_MASK);
	case L2CACHE:
		return 2 << ((read_c0_config2() >> MIPS_CONFIG2_SL_SHIFT) &
				MIPS_CONFIG2_SL_MASK);
	default:
		printk(BIOS_ERR, "%s: Error: unsupported cache type.\n",
				__func__);
		return 0;
	}
	return 0;
}

void perform_cache_operation(uintptr_t start, size_t size, uint8_t operation)
{
	u32 line_size, line_mask;
	uintptr_t end;

	line_size = get_cache_line_size((operation >> CACHE_TYPE_SHIFT) &
					CACHE_TYPE_MASK);
	if (!line_size)
		return;
	line_mask = ~(line_size-1);
	end = (start + (line_size - 1) + size) & line_mask;
	start &= line_mask;
	if ((operation & L2CACHE) == L2CACHE)
		write_c0_l23taglo(0);
	while (start < end) {
		switch (operation) {
		case CACHE_CODE(ICACHE, WB_INVD):
			cache_op(CACHE_CODE(ICACHE, WB_INVD), start);
			break;
		case CACHE_CODE(DCACHE, WB_INVD):
			cache_op(CACHE_CODE(DCACHE, WB_INVD), start);
			break;
		case CACHE_CODE(L2CACHE, WB_INVD):
			cache_op(CACHE_CODE(L2CACHE, WB_INVD), start);
			break;
		default:
			return;
		}
		start += line_size;
	}
	asm("sync");
}

void cache_invalidate_all(uintptr_t start, size_t size)
{
	perform_cache_operation(start, size, CACHE_CODE(ICACHE, WB_INVD));
	perform_cache_operation(start, size, CACHE_CODE(DCACHE, WB_INVD));
	perform_cache_operation(start, size, CACHE_CODE(L2CACHE, WB_INVD));
}

void arch_segment_loaded(uintptr_t start, size_t size, int flags)
{
	cache_invalidate_all(start, size);
	if (flags & SEG_FINAL)
		cache_invalidate_all((uintptr_t)_cbfs_cache, _cbfs_cache_size);
}
