/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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
#include <stddef.h>
#include <stdlib.h>
#include <symbols.h>
#include <soc/emi.h>

#define MAX_DRAM_ADDRESS ((uintptr_t)4 * GiB)

void *cbmem_top(void)
{
	return (void *)min((uintptr_t)_dram + sdram_size(), MAX_DRAM_ADDRESS);
}
