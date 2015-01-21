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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
#include <symbols.h>
#include <console/console.h>

/* Cache operations */

/*
 * __get_line_size:
 *	Read config register
 *	Isolate instruction cache line size
 *	Interpret value as per MIPS manual: 2 << value
 *	Return cache line size
 */
#define __get_line_size(cfg_no, cfg_sel, lshift, nobits)		\
({	int __res;							\
	__asm__ __volatile__(						\
		".set push\n\t"						\
		".set noreorder\n\t"					\
		".set mips32\n\t"					\
		"mfc0 $t5, "#cfg_no"," #cfg_sel"\n\t"			\
		".set mips0\n\t"					\
		"ext $t6, $t5," #lshift"," #nobits"\n\t"		\
		"li $t7, 2\n\t"						\
		"sllv %0, $t7, $t6\n\t"					\
		".set pop\n\t"						\
		: "=r" (__res));					\
	__res;								\
})

/* clear_L2tag: clear L23Tag register */
#define clear_L2tag()							\
({									\
	__asm__ __volatile__(						\
		".set push\n\t"						\
		".set noreorder\n\t"					\
		".set mips32\n\t"					\
		"mtc0 $zero, $28, 4\n\t"				\
		".set mips0\n\t"					\
		".set pop\n\t"						\
		);							\
})

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

static int get_cache_line(uint8_t type)
{
	switch (type) {
	case ICACHE: return get_icache_line();
	case DCACHE: return get_dcache_line();
	case L2CACHE: return get_L2cache_line();
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

	line_size = get_cache_line((operation >> CACHE_TYPE_SHIFT) &
					CACHE_TYPE_MASK);
	if (!line_size)
		return;
	line_mask = ~(line_size-1);
	end = (start + (line_size - 1) + size) & line_mask;
	start &= line_mask;
	if ((operation & L2CACHE) == L2CACHE)
		clear_L2tag();
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
