/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <types.h>

/*
 * Provide platform-independent backend implementation for __builtin_clz() in
 * <lib.h> in case GCC does not have an assembly version for this arch.
 */

#if !IS_ENABLED(CONFIG_ARCH_X86) /* work around lack of --gc-sections on x86 */
int __clzsi2(u32 a);
int __clzsi2(u32 a)
{
	static const u8 four_bit_table[] = {
		[0x0] = 4, [0x1] = 3, [0x2] = 2, [0x3] = 2,
		[0x4] = 1, [0x5] = 1, [0x6] = 1, [0x7] = 1,
		[0x8] = 0, [0x9] = 0, [0xa] = 0, [0xb] = 0,
		[0xc] = 0, [0xd] = 0, [0xe] = 0, [0xf] = 0,
	};
	int r = 0;

	if (!(a & (0xffff << 16))) {
		r += 16;
		a <<= 16;
	}

	if (!(a & (0xff << 24))) {
		r += 8;
		a <<= 8;
	}

	if (!(a & (0xf << 28))) {
		r += 4;
		a <<= 4;
	}

	return r + four_bit_table[a >> 28];
}
#endif
