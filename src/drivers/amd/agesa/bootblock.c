/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <halt.h>
#include <timestamp.h>
#include <amdblocks/amd_pci_mmconf.h>
#include <amdblocks/biosram.h>
#include <arch/bootblock.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/lapic.h>

#define EARLY_VMTRR_FLASH 6

static void set_early_mtrrs(void)
{
	/* Cache the ROM to speed up booting */
	set_var_mtrr(EARLY_VMTRR_FLASH, OPTIMAL_CACHE_ROM_BASE,
		     OPTIMAL_CACHE_ROM_SIZE, MTRR_TYPE_WRPROT);
}

void bootblock_soc_early_init(void)
{
	bootblock_early_southbridge_init();
}

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	enable_pci_mmconf();
	set_early_mtrrs();

	if (CONFIG(UDELAY_LAPIC))
		enable_lapic();

	bootblock_main_with_basetime(base_timestamp);
}

asmlinkage void ap_bootblock_c_entry(void)
{
	enable_pci_mmconf();
	set_early_mtrrs();

	if (CONFIG(UDELAY_LAPIC))
		enable_lapic();

	void (*ap_romstage_entry)(void) = get_ap_entry_ptr();
	ap_romstage_entry(); /* execution does not return */
	halt();
}
