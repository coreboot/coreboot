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
 */

#include <memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	DRAM_START(0x40000000)
	BOOTBLOCK(0x402f0400, 20K)
	ROMSTAGE(0x402f5400, 90K)
	STACK(0x4030be00, 4K)
	RAMSTAGE(0x80200000, 192K)

	/* TODO: Implement MMU support and move TTB to a better location. */
	TTB(0x81000000, 16K)

#ifdef OMAP_HEADER
	.header : {
		*(.header);
	} : to_load

	/DISCARD/ : {
		*(*)
	}
#endif
}
