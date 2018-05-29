/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Jonathan Neuschäfer
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

#include <memlayout.h>
#include <soc/addressmap.h>

#include <arch/header.ld>

#define L2LIM_START(addr) SYMBOL(l2lim, addr)
#define L2LIM_END(addr) SYMBOL(el2lim, addr)

SECTIONS
{
	L2LIM_START(FU540_L2LIM)
	BOOTBLOCK(FU540_L2LIM, 64K)
	STACK(FU540_L2LIM + 64K, 4K)
	PRERAM_CBMEM_CONSOLE(FU540_L2LIM + 68K, 8K)
	ROMSTAGE(FU540_L2LIM + 128K, 128K)
	L2LIM_END(FU540_L2LIM + 2M)

	DRAM_START(FU540_DRAM)
	RAMSTAGE(FU540_DRAM, 256K)
}
