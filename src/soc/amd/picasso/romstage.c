/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/cpu.h>
#include <arch/romstage.h>
#include <arch/acpi.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <elog.h>
#include <soc/romstage.h>

void __weak mainboard_romstage_entry_s3(int s3_resume)
{
	/* By default, don't do anything */
}

asmlinkage void car_stage_entry(void)
{
	uintptr_t top_of_mem;
	int s3_resume;

	post_code(0x40);
	console_init();

	post_code(0x41);
	s3_resume = acpi_s3_resume_allowed() && acpi_is_wakeup_s3();
	mainboard_romstage_entry_s3(s3_resume);
	elog_boot_notify(s3_resume);

	post_code(0x42);
	u32 val = cpuid_eax(1);
	printk(BIOS_DEBUG, "Family_Model: %08x\n", val);

	post_code(0x43);
	top_of_mem = ALIGN_DOWN(rdmsr(TOP_MEM).lo, 8 * MiB);
	backup_top_of_low_cacheable(top_of_mem);

	post_code(0x44);
	if (cbmem_recovery(s3_resume))
		printk(BIOS_CRIT, "Failed to recover cbmem\n");
	if (romstage_handoff_init(s3_resume))
		printk(BIOS_ERR, "Failed to set romstage handoff data\n");

	post_code(0x45);
	run_ramstage();

	post_code(0x50); /* Should never see this post code. */
}
