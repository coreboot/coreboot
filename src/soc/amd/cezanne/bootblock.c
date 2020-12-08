/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_mmconf.h>
#include <bootblock_common.h>
#include <cpu/x86/tsc.h>
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
}

void bootblock_soc_init(void)
{
}
