/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <halt.h>
#include <amdblocks/amd_pci_mmconf.h>
#include <amdblocks/biosram.h>
#include <arch/bootblock.h>
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
