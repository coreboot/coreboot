/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stdint.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <soc/southbridge.h>
#include <soc/i2c.h>
#include <amdblocks/amd_pci_mmconf.h>

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	enable_pci_mmconf();

	bootblock_main_with_basetime(base_timestamp);
}

void bootblock_soc_early_init(void)
{
	sb_reset_i2c_slaves();
	fch_pre_init();
}

void bootblock_soc_init(void)
{
	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	fch_early_init();
	i2c_soc_early_init();
}
