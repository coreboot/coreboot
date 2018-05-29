/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#include <arch/header.ld>

SECTIONS
{
	SRAM_START(0x0)
	/* eGON.BT0: 32 bytes */
	BOOTBLOCK(0x20, 0x5fa0)
	STACK(0x6000, 8K)
	SRAM_END(0x8000)

	DRAM_START(0x40000000)
	RAMSTAGE(0x40000000, 16M)
	ROMSTAGE(0x41000000, 108K)

	/* TODO: Implement MMU support and move TTB to a better location. */
	TTB(0x42000000, 16K)
}
