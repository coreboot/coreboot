/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

// TODO: fill in these blanks for Power9.
SECTIONS
{
	DRAM_START(0x0)
	BOOTBLOCK(0, 64K)
	ROMSTAGE(0x120000, 128K)
	STACK(0x140000, 0x3ff00)
	PRERAM_CBMEM_CONSOLE(0x180000, 8K)
	FMAP_CACHE(0x182000, 2K)
	CBFS_MCACHE(0x182800, 8K)
	RAMSTAGE(0x200000, 16M)
}
