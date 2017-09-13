/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Advanced Micro Devices, Inc.
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

#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <agesawrapper.h>
#include <agesawrapper_call.h>
#include <soc/northbridge.h>
#include <soc/southbridge.h>
#include <amdblocks/psp.h>

asmlinkage void car_stage_entry(void)
{
	msr_t base, mask;
	msr_t mtrr_cap = rdmsr(MTRR_CAP_MSR);
	int vmtrrs = mtrr_cap.lo & MTRR_CAP_VCNT;
	int i;

	console_init();

	post_code(0x40);
	AGESAWRAPPER(amdinitpost);

	post_code(0x41);
	/*
	 * TODO: This is a hack to work around current AGESA behavior.  AGESA
	 *       needs to change to reflect that coreboot owns the MTRRs.
	 *
	 * After setting up DRAM, AGESA also completes the configuration of the
	 * MTRRs, setting regions to WB.  Anything written to memory between
	 * now and and when CAR is dismantled will be in cache and lost.  For
	 * now, set the regions UC to ensure the writes get to DRAM.
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

	post_code(0x42);
	psp_notify_dram();

	post_code(0x43);
	cbmem_initialize_empty();

	/*
	 * This writes contents to DRAM backing before teardown.
	 * todo: move CAR teardown to postcar implementation and
	 *       relocate amdinitenv to ramstage.
	 */
	chipset_teardown_car();

	post_code(0x44);
	AGESAWRAPPER(amdinitenv);

	post_code(0x50);
	run_ramstage();

	post_code(0x54);  /* Should never see this post code. */
}
