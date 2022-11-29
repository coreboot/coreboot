/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	SRAM_START(0x402f0400)
	BOOTBLOCK(., 22K)
	FMAP_CACHE(., 2K)
	CBFS_MCACHE(., 8K)
	ROMSTAGE(., 42K)
	PRERAM_CBFS_CACHE(., 20K)

	STACK(., 4K)
	. = ALIGN(16K);
	TTB(., 16K)
	SRAM_END(0x40310000)

	DRAM_START(0x80000000)
	RAMSTAGE(0x80000000, 2M)
	POSTRAM_CBFS_CACHE(0x80200000, 32M)

#ifdef OMAP_HEADER
	.header : {
		*(.header);
	} : to_load

	/DISCARD/ : {
		*(*)
	}
#endif
}
