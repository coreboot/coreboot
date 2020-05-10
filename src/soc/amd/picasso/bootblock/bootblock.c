/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <symbols.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <soc/southbridge.h>
#include <soc/i2c.h>
#include <amdblocks/amd_pci_mmconf.h>

static void set_caching(void)
{
	msr_t deftype = {0, 0};
	int mtrr;

	/* Disable fixed and variable MTRRs while we setup */
	wrmsr(MTRR_DEF_TYPE_MSR, deftype);

	clear_all_var_mtrr();

	mtrr = get_free_var_mtrr();
	if (mtrr >= 0)
		set_var_mtrr(mtrr, FLASH_BASE_ADDR, CONFIG_ROM_SIZE, MTRR_TYPE_WRPROT);

	mtrr = get_free_var_mtrr();
	if (mtrr >= 0)
		set_var_mtrr(mtrr, (unsigned int)_bootblock, REGION_SIZE(bootblock),
			     MTRR_TYPE_WRBACK);

	/* Enable variable MTRRs. Fixed MTRRs are left disabled since they are not used. */
	deftype.lo |= MTRR_DEF_TYPE_EN | MTRR_TYPE_UNCACHEABLE;
	wrmsr(MTRR_DEF_TYPE_MSR, deftype);

	enable_cache();
}

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	set_caching();
	enable_pci_mmconf();

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
