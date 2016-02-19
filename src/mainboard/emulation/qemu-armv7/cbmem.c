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

#include <stddef.h>
#include <cbmem.h>
#include <symbols.h>
#include <arch/io.h>
#include "mainboard.h"

#define PATTERN1 0x55
#define PATTERN2 0xaa

/* Returns 1 if mebibyte mb is present and 0 otherwise.  */
static int probe_mb(int mb)
{
	char *ptr = (char *) 0x60000000 + (mb << 20) + 0xfffff;
	char old;
	if (ptr < (char *) &_eprogram) {
		/* Don't probe below _end to avoid accidentally clobering
		   oneself.  */
		return 1;
	}

	old = read8(ptr);
	write8(ptr, PATTERN1);
	if (read8(ptr) != PATTERN1)
		return 0;
	write8(ptr, PATTERN2);
	if (read8(ptr) != PATTERN2)
		return 0;
	write8(ptr, old);
	return 1;
}

int probe_ramsize(void)
{
	int i;
	int discovered = 0;
	static int saved_result;
	if (saved_result)
		return saved_result;
	/* Compact binary search.  */
	/* 1 GiB is the largest supported RAM by this machine.  */
	for (i = 9; i >= 0; i--)
		if (probe_mb(discovered | (1 << i)))
			discovered |= (1 << i);
	discovered++;
	saved_result = discovered;
	return discovered;
}

void *cbmem_top(void)
{
	return _dram + (probe_ramsize() << 20);
}
