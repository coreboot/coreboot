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

/*
 * Memory map for qemu vexpress-a9:
 *
 * 0x0000_0000: jump instruction (by qemu)
 * 0x0001_0000: bootblock (entry of kernel / firmware)
 * 0x0002_0000: romstage, assume up to 128KB in size.
 * 0x0007_ff00: stack pointer
 * 0x0010_0000: CBFS header
 * 0x0011_0000: CBFS data
 * 0x0100_0000: reserved for ramstage
 * 0x1000_0000: I/O map address
 */

SECTIONS
{
	/* TODO: does this thing emulate SRAM? */

	BOOTBLOCK(0x10000, 64K)
	ROMSTAGE(0x20000, 128K)
	STACK(0x000FC000, 16K)

	DRAM_START(0x01000000)
	RAMSTAGE(0x01000000, 16M)
}
