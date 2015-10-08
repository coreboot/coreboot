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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <memlayout.h>

#include <arch/header.ld>

/*
 * Note: The BootROM uses the address range [0x4000_0000:0x4000_E000) itself,
 * so the bootblock loading address must be placed after that. After the
 * handoff that area may be reclaimed for other uses, e.g. CBFS cache.
 * TODO: Did this change on Tegra132? What's the new valid range?
 */

SECTIONS
{
	SRAM_START(0x40000000)
	PRERAM_CBMEM_CONSOLE(0x40000000, 8K)
	PRERAM_CBFS_CACHE(0x40002000, 72K)
	VBOOT2_WORK(0x40014000, 16K)
	STACK(0x40018000, 2K)
	TIMESTAMP(0x40018800, 2K)
	BOOTBLOCK(0x40019000, 22K)
	VERSTAGE(0x4001e800, 55K)
	ROMSTAGE(0x4002c400, 77K)
	SRAM_END(0x40040000)

	DRAM_START(0x80000000)
	POSTRAM_CBFS_CACHE(0x80100000, 1M)
	RAMSTAGE(0x80200000, 256K)
}
