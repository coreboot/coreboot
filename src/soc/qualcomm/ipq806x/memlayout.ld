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
	/* TODO: add SRAM_START(), SRAM_END() and REGION(reserved_sbl) */
	TTB(0x2A05C000, 48K)

	DRAM_START(0x40000000)
	CBFS_CACHE(0x405CC000, 192K)
	STACK(0x405FC000, 16K)
	/* TODO: "256K bytes left for TZBSP"... what does that mean? */
	BOOTBLOCK(0x40600000, 32K)
	PRERAM_CBMEM_CONSOLE(0x40618000, 8K)
	ROMSTAGE(0x40620000, 128K)
	RAMSTAGE(0x40640000, 128K)
	DMA_COHERENT(0x5A000000, 2M)
}
