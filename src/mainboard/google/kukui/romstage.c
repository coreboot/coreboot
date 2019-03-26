/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#include <arch/stages.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/mt6358.h>
#include <soc/pll.h>
#include <soc/rtc.h>

#include "early_init.h"

void platform_romstage_main(void)
{
	/* This will be done in verstage if CONFIG_VBOOT is enabled. */
	if (!CONFIG(VBOOT))
		mainboard_early_init();

	mt6358_init();
	/* Adjust VSIM2 down to 2.7V because it is shared with IT6505. */
	pmic_set_vsim2_cali(2700);
	mt_pll_raise_ca53_freq(1989 * MHz);
	rtc_boot();
	mt_mem_init(get_sdram_config());
	mtk_mmu_after_dram();
}
