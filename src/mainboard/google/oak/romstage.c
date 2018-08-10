/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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
#include <boardid.h>
#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/mt6391.h>
#include <soc/pll.h>
#include <soc/rtc.h>
#include <timer.h>

void platform_romstage_main(void)
{
	int stabilize_usec;
	struct stopwatch sw;

	rtc_boot();

	/* Raise CPU voltage to allow higher frequency */
	stabilize_usec = mt6391_configure_ca53_voltage(1125000);

	stopwatch_init_usecs_expire(&sw, stabilize_usec);

	/* init memory */
	mt_mem_init(get_sdram_config());

	stopwatch_wait_until_expired(&sw);

	/* Set to maximum frequency */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 5)
		mt_pll_raise_ca53_freq(1600 * MHz);
	else
		mt_pll_raise_ca53_freq(1700 * MHz);

	mtk_mmu_after_dram();
}
