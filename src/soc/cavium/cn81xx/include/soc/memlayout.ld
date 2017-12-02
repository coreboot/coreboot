/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
 * Copyright 2017-present Facebook, Inc.
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

SECTIONS
{
	DRAM_START(0x00000000)
	/* FIXME: Place BL31 in first 1MiB */

	/* bootblock-custom.S does setup CAR from SRAM_START to SRAM_END */
	SRAM_START(BOOTROM_OFFSET)
	STACK(BOOTROM_OFFSET, 16K)
	TIMESTAMP(BOOTROM_OFFSET + 0x4000, 4K)
	PRERAM_CBFS_CACHE(BOOTROM_OFFSET + 0x6000, 8K)
	PRERAM_CBMEM_CONSOLE(BOOTROM_OFFSET + 0x8000, 8K)

	BOOTBLOCK(BOOTROM_OFFSET + 0x20000, 64K)
	ROMSTAGE(BOOTROM_OFFSET + 0x40000, 256K)
	SRAM_END(BOOTROM_OFFSET + 0x80000)
	TTB(BOOTROM_OFFSET + 0x80000, 128K)
	RAMSTAGE(BOOTROM_OFFSET + 0xa0000, 512K)

	/* Leave some space for the payload */
	POSTRAM_CBFS_CACHE(0x2000000, 16M)
}
