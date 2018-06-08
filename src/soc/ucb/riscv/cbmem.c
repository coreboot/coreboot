/*
 * This file is part of the coreboot project.
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

#include <cbmem.h>

void *cbmem_top(void)
{
	uintptr_t base;
	size_t size;

	/* Use dummy values until we can query the memory size again */
	//query_mem(configstring(), &base, &size);
	base = 0x80000000;
	size = 128 * MiB;

	return (void *)(base + size);
}
