/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <device/pci_ops.h>
#include <console/console.h>
#include <southbridge/intel/common/pmclib.h>
#include <northbridge/intel/x4x/x4x.h>
#include <arch/romstage.h>

#if CONFIG(SOUTHBRIDGE_INTEL_I82801JX)
#include <southbridge/intel/i82801jx/i82801jx.h>
#elif CONFIG(SOUTHBRIDGE_INTEL_I82801GX)
#include <southbridge/intel/i82801gx/i82801gx.h>
#endif

__weak void mb_pre_raminit_setup(int s3_resume)
{
}

void mainboard_romstage_entry(void)
{
	u8 spd_addr_map[4] = {};
	u8 boot_path = 0;
	u8 s3_resume;

#if CONFIG(SOUTHBRIDGE_INTEL_I82801JX)
	i82801jx_early_init();
#elif CONFIG(SOUTHBRIDGE_INTEL_I82801GX)
	i82801gx_early_init();
#endif

	x4x_early_init();

	s3_resume = southbridge_detect_s3_resume();
	mb_pre_raminit_setup(s3_resume);

	if (s3_resume)
		boot_path = BOOT_PATH_RESUME;
	if (MCHBAR32(PMSTS_MCHBAR) & PMSTS_WARM_RESET)
		boot_path = BOOT_PATH_WARM_RESET;

	mb_get_spd_map(spd_addr_map);
	sdram_initialize(boot_path, spd_addr_map);

	x4x_late_init(s3_resume);

	printk(BIOS_DEBUG, "x4x late init complete\n");
}
