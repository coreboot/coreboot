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

#include <console/console.h>
#include <cbmem.h>

void *cbmem_top(void)
{
	// TODO: find out how the lowrisc SOC decides to provide
	// this information, when they know.
	printk(BIOS_SPEW, "Returning hard-coded 128M; fix me\n");
	return (void *)((uintptr_t)(2ULL*GiB+128*MiB));
}
