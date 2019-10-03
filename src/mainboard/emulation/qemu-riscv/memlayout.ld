/* SPDX-License-Identifier: GPL-2.0-only */

#include <memlayout.h>
#include <arch/header.ld>
#include <mainboard/addressmap.h>

// Stages start after CBFS in DRAM
#define STAGES_START (QEMU_VIRT_DRAM + CONFIG_ROM_SIZE)

SECTIONS
{
	// the virt target doesn't emulate flash and just puts the CBFS into DRAM.
	// fake SRAM where CBFS resides. It's only done for better integration.
	SRAM_START(QEMU_VIRT_DRAM)
	BOOTBLOCK(QEMU_VIRT_DRAM, 64K)
	// CBFS goes here
	SRAM_END(STAGES_START)
	DRAM_START(STAGES_START)

#if ENV_ROMSTAGE
	ROMSTAGE(STAGES_START, 128K)
#endif
#if ENV_RAMSTAGE
	REGION(opensbi, STAGES_START, 128K, 4K)
#endif
	PRERAM_CBMEM_CONSOLE(STAGES_START + 128K, 8K)
	FMAP_CACHE(STAGES_START + 136K, 2K)
	CBFS_MCACHE(STAGES_START + 138K, 8K)
	RAMSTAGE(STAGES_START + 200K, 16M)
	STACK(STAGES_START + 200K + 16M, 4K)
}
