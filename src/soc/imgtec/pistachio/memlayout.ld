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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <memlayout.h>

#include <arch/header.ld>

/* TODO: This should be revised by someone who understands the SoC better. */

SECTIONS
{
	CBFS_CACHE(0x0, 0) /* TODO: fix this, it was already broken before!!! */

	DRAM_START(0x80000000)
	RAMSTAGE(0x80000000, 128K)

	/* TODO: Does this SoC use SRAM? Add SRAM_START() and SRAM_END(). */
	BOOTBLOCK(0x9B000000, 16K)
	ROMSTAGE(0x9B004000, 40K)
	STACK(0x9B00E000, 6K)
	PRERAM_CBMEM_CONSOLE(0x9B00F800, 3K)
}
