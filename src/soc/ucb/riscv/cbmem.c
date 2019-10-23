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
#include <symbols.h>
#include <ramdetect.h>

void *cbmem_top_chipset(void)
{
	return _dram + (probe_ramsize((uintptr_t)_dram, CONFIG_DRAM_SIZE_MB) * MiB);
}
