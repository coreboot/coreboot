/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/amd_pci_mmconf.h>
#include <amdblocks/cpu.h>
#include <bootblock_common.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/tsc.h>
#include <psp_verstage/psp_transfer.h>
#include <soc/southbridge.h>
#include <stdint.h>

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	enable_pci_mmconf();
	early_cache_setup();
	write_resume_eip();

	if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK))
		boot_with_psp_timestamp(base_timestamp);

	/*
	 * if VBOOT_STARTS_BEFORE_BOOTBLOCK is not selected or
	 * previous step did nothing, proceed with normal bootblock main.
	 */
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

	if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		verify_psp_transfer_buf();
		show_psp_transfer_info();
	}

	fch_early_init();
}
