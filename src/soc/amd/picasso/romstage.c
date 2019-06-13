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
#include <arch/acpi.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
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

void __weak mainboard_romstage_entry(int s3_resume)
{
	/* By default, don't do anything */
}

asmlinkage void car_stage_entry(void)
{
	struct postcar_frame pcf;
	uintptr_t top_of_ram;
	void *smm_base;
	size_t smm_size;
	uintptr_t tseg_base;
	int s3_resume = acpi_s3_resume_allowed() && acpi_is_wakeup_s3();

	console_init();

	mainboard_romstage_entry(s3_resume);

	if (!s3_resume) {
		post_code(0x40);

		if (CONFIG(ELOG_BOOT_COUNT))
			boot_count_increment();
	} else {
		printk(BIOS_INFO, "S3 detected\n");
		post_code(0x60);
	}

	post_code(0x43);
	if (cbmem_recovery(s3_resume))
		printk(BIOS_CRIT, "Failed to recover cbmem\n");
	if (romstage_handoff_init(s3_resume))
		printk(BIOS_ERR, "Failed to set romstage handoff data\n");

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

	/*
	 * Cache the TSEG region at the top of ram. This region is
	 * not restricted to SMM mode until SMM has been relocated.
	 * By setting the region to cacheable it provides faster access
	 * when relocating the SMM handler as well as using the TSEG
	 * region for other purposes.
	 */
	smm_region_info(&smm_base, &smm_size);
	tseg_base = (uintptr_t)smm_base;
	postcar_frame_add_mtrr(&pcf, tseg_base, smm_size, MTRR_TYPE_WRBACK);

	post_code(0x45);
	run_postcar_phase(&pcf);

	post_code(0x50);  /* Should never see this post code. */
}
