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

#include <arch/io.h>
#include <arch/cpu.h>
#include <arch/acpi.h>
#include <compiler.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <device/device.h>
#include <chip.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <elog.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>
#include <soc/northbridge.h>
#include <soc/romstage.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>

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
	msr_t base, mask;
	msr_t mtrr_cap = rdmsr(MTRR_CAP_MSR);
	int vmtrrs = mtrr_cap.lo & MTRR_CAP_VCNT;
	int s3_resume = acpi_s3_resume_allowed() && acpi_is_wakeup_s3();
	int i;

	/*
	 * When moving AGESA calls to romstage, place the call to
	 * bsp_agesa_call() here.
	 */

	console_init();

	mainboard_romstage_entry(s3_resume);

	if (!s3_resume) {
		post_code(0x40);
		do_agesawrapper(agesawrapper_amdinitpost, "amdinitpost");

		post_code(0x41);
		/*
		 * TODO: This is a hack to work around current AGESA behavior.
		 *       AGESA needs to change to reflect that coreboot owns
		 *       the MTRRs.
		 *
		 * After setting up DRAM, AGESA also completes the configuration
		 * of the MTRRs, setting regions to WB.  Anything written to
		 * memory between now and and when CAR is dismantled will be
		 * in cache and lost.  For now, set the regions UC to ensure
		 * the writes get to DRAM.
		 */
		for (i = 0 ; i < vmtrrs ; i++) {
			base = rdmsr(MTRR_PHYS_BASE(i));
			mask = rdmsr(MTRR_PHYS_MASK(i));
			if (!(mask.lo & MTRR_PHYS_MASK_VALID))
				continue;

			if ((base.lo & 0x7) == MTRR_TYPE_WRBACK) {
				base.lo &= ~0x7;
				base.lo |= MTRR_TYPE_UNCACHEABLE;
				wrmsr(MTRR_PHYS_BASE(i), base);
			}
		}
		/* Disable WB from to region 4GB-TOM2. */
		msr_t sys_cfg = rdmsr(SYSCFG_MSR);
		sys_cfg.lo &= ~SYSCFG_MSR_TOM2WB;
		wrmsr(SYSCFG_MSR, sys_cfg);
		if (IS_ENABLED(CONFIG_ELOG_BOOT_COUNT))
			boot_count_increment();
	} else {
		printk(BIOS_INFO, "S3 detected\n");
		post_code(0x60);
		do_agesawrapper(agesawrapper_amdinitresume, "amdinitresume");

		post_code(0x61);
	}

	post_code(0x42);
	psp_notify_dram();

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

void SetMemParams(AMD_POST_PARAMS *PostParams)
{
	const struct soc_amd_stoneyridge_config *cfg;
	const struct device *dev = dev_find_slot(0, GNB_DEVFN);

	if (!dev || !dev->chip_info) {
		printk(BIOS_ERR, "ERROR: Cannot find SoC devicetree config\n");
		/* In case of a BIOS error, only attempt to set UMA. */
		PostParams->MemConfig.UmaMode = IS_ENABLED(CONFIG_GFXUMA) ?
					UMA_AUTO : UMA_NONE;
		return;
	}

	cfg = dev->chip_info;

	PostParams->MemConfig.EnableMemClr = cfg->dram_clear_on_reset;

	switch (cfg->uma_mode) {
	case UMAMODE_NONE:
		PostParams->MemConfig.UmaMode = UMA_NONE;
		break;
	case UMAMODE_SPECIFIED_SIZE:
		PostParams->MemConfig.UmaMode = UMA_SPECIFIED;
		/* 64 KiB blocks. */
		PostParams->MemConfig.UmaSize = cfg->uma_size / (64 * KiB);
		break;
	case UMAMODE_AUTO_LEGACY:
		PostParams->MemConfig.UmaMode = UMA_AUTO;
		PostParams->MemConfig.UmaVersion = UMA_LEGACY;
		break;
	case UMAMODE_AUTO_NON_LEGACY:
		PostParams->MemConfig.UmaMode = UMA_AUTO;
		PostParams->MemConfig.UmaVersion = UMA_NON_LEGACY;
		break;
	}
}
