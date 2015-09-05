/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <cpu/cpu.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>

#include <cbmem.h>

#include "ram_calc.h"

#if !IS_ENABLED(CONFIG_LATE_CBMEM_INIT)
uint64_t get_uma_memory_size(uint64_t topmem)
{
	uint64_t uma_size = 0;
	if (IS_ENABLED(CONFIG_GFXUMA)) {
		/* refer to UMA Size Consideration in 780 BDG. */
		if (topmem >= 0x40000000)	/* 1GB and above system memory */
			uma_size = 0x10000000;	/* 256M recommended UMA */

		else if (topmem >= 0x20000000)	/* 512M - 1023M system memory */
			uma_size = 0x8000000;	/* 128M recommended UMA */

		else if (topmem >= 0x10000000)	/* 256M - 511M system memory */
			uma_size = 0x4000000;	/* 64M recommended UMA */
	}

	return uma_size;
}

void *cbmem_top(void)
{
	uint32_t topmem = rdmsr(TOP_MEM).lo;

	return (void *) topmem - get_uma_memory_size(topmem);
}
#endif
