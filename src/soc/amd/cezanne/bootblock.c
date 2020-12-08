/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_mmconf.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <cpu/x86/tsc.h>
#include <soc/southbridge.h>
#include <stdint.h>

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	enable_pci_mmconf();

	/*
	 * base_timestamp is raw tsc value. We need to divide by tsc_freq_mhz
	 * to get micro-seconds granularity.
	 */
	base_timestamp /= tsc_freq_mhz();

	bootblock_main_with_basetime(base_timestamp);
}

void bootblock_soc_early_init(void)
{
	fch_pre_init();
}

void bootblock_soc_init(void)
{
	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);
	fch_early_init();
}
