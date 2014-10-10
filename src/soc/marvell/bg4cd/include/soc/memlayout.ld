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
#include <vendorcode/google/chromeos/vboot2/memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	DRAM_START(0x00000000)
	RAMSTAGE(0x00200000, 128K)
	POSTRAM_CBFS_CACHE(0x01000000, 1M)

	SRAM_START(0x80000000)
	TTB(0x80000000, 16K)
	BOOTBLOCK(0x80004004, 16K - 4)
	VBOOT2_WORK(0x80008000, 16K)
	OVERLAP_VERSTAGE_ROMSTAGE(0x8000C000, 40K)
	PRERAM_CBFS_CACHE(0x80016000, 4K)
	STACK(0x80017000, 4K)
	SRAM_END(0x80018000)
}
