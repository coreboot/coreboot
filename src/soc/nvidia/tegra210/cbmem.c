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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <cbmem.h>
#include <soc/addressmap.h>

void *cbmem_top(void)
{
	static uintptr_t addr;

	if (addr == 0) {
		uintptr_t begin_mib;
		uintptr_t end_mib;

		memory_in_range_below_4gb(&begin_mib, &end_mib);
		/* Make sure we consume everything up to 4GIB. */
		if (end_mib == 4096)
			addr = ~(uint32_t)0;
		else
			addr = end_mib << 20;
	}

	return (void *)addr;
}
