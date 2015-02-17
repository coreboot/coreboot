/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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
#include <vendorcode/google/chromeos/memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	DRAM_START(0x00000000)
	RAMSTAGE(0x00200000, 128K)
	POSTRAM_CBFS_CACHE(0x01000000, 1M)

	SRAM_START(0x02000000)
	REGION(reserved_for_system_status, 0x02000000, 4K, 4)
	TTB(0x02004000, 16K)		/* must be aligned to 16K */
	TTB_SUBTABLES(0x02008000, 1K)
	REGION(reserved_for_maskrom, 0x02009400, 4K, 4)
	BOOTBLOCK(0x0200A440, 18K)
	PRERAM_CBMEM_CONSOLE(0x0200F000, 4K)
	VBOOT2_WORK(0x02010000, 16K)
	VERSTAGE(0x02014000, 48K)
	ROMSTAGE(0x02020000, 48K)
	PRERAM_CBFS_CACHE(0x0202C000, 1K)
	CBFS_HEADER_OFFSET(0x0202C800)
	STACK(0x0202D000, 12K)
	REGION(reserved_for_secure_service_api, 0x0203F000, 4K, 4)
	SRAM_END(0x02040000)
}
