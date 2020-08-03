/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	SRAM_START(0x402f0400)
	BOOTBLOCK(0x402f0400, 20K)
	ROMSTAGE(0x402f5400, 88K)
	FMAP_CACHE(0x4030b400, 2K)
	STACK(0x4030be00, 4K)
	SRAM_END(0x40310000)
	DRAM_START(0x80000000)

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
