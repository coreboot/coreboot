/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <device/pci_ops.h>
#include <arch/cpu.h>
#include <arch/romstage.h>
#include <arch/acpi.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cpu/amd/mtrr.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <elog.h>
#include <soc/northbridge.h>
#include <soc/romstage.h>
#include <soc/southbridge.h>

#include "chip.h"

void __weak mainboard_romstage_entry_s3(int s3_resume)
{
	/* By default, don't do anything */
}

asmlinkage void car_stage_entry(void)
{
	struct postcar_frame pcf;
	uintptr_t top_of_ram;
	int s3_resume = acpi_s3_resume_allowed() && acpi_is_wakeup_s3();

	console_init();

	mainboard_romstage_entry_s3(s3_resume);
	elog_boot_notify(s3_resume);

	if (!s3_resume) {
		post_code(0x40);
	} else {
		printk(BIOS_INFO, "S3 detected\n");
		post_code(0x60);
	}

	post_code(0x43);
	if (cbmem_recovery(s3_resume))
		printk(BIOS_CRIT, "Failed to recover cbmem\n");
	if (romstage_handoff_init(s3_resume))
		printk(BIOS_ERR, "Failed to set romstage handoff data\n");

	if (CONFIG(SMM_TSEG))
		smm_list_regions();

	post_code(0x44);
	if (postcar_frame_init(&pcf, 1 * KiB))
		die("Unable to initialize postcar frame.\n");

	/*
	 * We need to make sure ramstage will be run cached. At this point exact
	 * location of ramstage in cbmem is not known. Instruct postcar to cache
	 * 16 megs under cbmem top which is a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t) cbmem_top();
	postcar_frame_add_mtrr(&pcf, top_of_ram - 16*MiB, 16*MiB,
		MTRR_TYPE_WRBACK);

	/* Cache the memory-mapped boot media. */
	postcar_frame_add_romcache(&pcf, MTRR_TYPE_WRPROT);

	/* Cache the TSEG region */
	postcar_enable_tseg_cache(&pcf);

	post_code(0x45);
	run_postcar_phase(&pcf);

	post_code(0x50);  /* Should never see this post code. */
}
