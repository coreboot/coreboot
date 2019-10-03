/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

// TODO: fill in these blanks for Power8.
SECTIONS
{
	DRAM_START(0x0)
	BOOTBLOCK(0x100, 64K)
	ROMSTAGE(0x20000, 128K)
	STACK(0x40000, 0x3ff00)
	PRERAM_CBMEM_CONSOLE(0x80000, 8K)
	FMAP_CACHE(0x82000, 2K)
	CBFS_MCACHE(0x82800, 8K)
	RAMSTAGE(0x100000, 16M)
}
