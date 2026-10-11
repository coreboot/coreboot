/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <memlayout.h>
#include <arch/header.ld>

SECTIONS
{
	DRAM_START(0x80000000)
	REGION(mm_comm_buffer, 0x88300000, 0x100000, 16)
	STACK(0x91100000, 128K)
	BOOTBLOCK(0x92000000, 64K)
	//BOOTBLOCK(0xe0000000, 64K) // fun errors when linking here
	CBFS_MCACHE(0xe0020000, 8K)
	FMAP_CACHE(0xe0022000 , 4K)
	TIMESTAMP(0xe0023000, 4K)
	RAMSTAGE(0xe00a0000, 16M)

	CBFS_CACHE(0xe11f0000, 1M)
	TTB(0xe5000000, 256K)
}
