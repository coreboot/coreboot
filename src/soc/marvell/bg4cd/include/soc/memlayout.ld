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

SECTIONS
{
	SRAM_START(0x20000)

	BOOTBLOCK(0x20000, 40K)
	/* there is no VERSTAGE because it's built into bootblock */

	PRERAM_CBFS_CACHE(0x2A000, 8K)
	STACK(0x2C000, 8K)
	VBOOT2_WORK(0x2E000, 16K)

	SRAM_END(0x40000)

	DRAM_START(0x40000)

	RAMSTAGE(0x40000, 128K)
	POSTRAM_CBFS_CACHE(0x60000, 1M)

	ROMSTAGE(0xF7A40000, 32K)
	TTB(0xF7A48000, 16K)
}
