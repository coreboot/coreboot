/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_mmconf.h>
#include <bootblock_common.h>
#include <stdint.h>

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	enable_pci_mmconf();
}

void bootblock_soc_early_init(void)
{
}

void bootblock_soc_init(void)
{
}
