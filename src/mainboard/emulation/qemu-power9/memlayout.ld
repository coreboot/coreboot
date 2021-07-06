/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	DRAM_START(0x0)

	BOOTBLOCK(0, 32K)

	ROMSTAGE(0x1f00000, 1M)

#if !ENV_RAMSTAGE
	STACK(0x2000000, 32K)
#endif

	FMAP_CACHE(0x2108000, 4K)
	CBFS_MCACHE(0x2109000, 8K)
	TIMESTAMP(0x210b000, 4K)
	CBFS_CACHE(0x210c000, 512K)
	PRERAM_CBMEM_CONSOLE(0x218c000, 128K)

	/* By default all memory addresses are affected by the value of HRMOR
	 * (Hypervisor Real Mode Offset Register) which is ORed to them. HRMOR
	 * has initial value of 0x8000000 in QEMU and is changed to 0 in
	 * ramstage. This means that before ramstage 0 actually points to
	 * 0x8000000. */
#if ENV_RAMSTAGE
	STACK(0xa000000, 32K)
#endif
	RAMSTAGE(0xa008000, 1M)
}
