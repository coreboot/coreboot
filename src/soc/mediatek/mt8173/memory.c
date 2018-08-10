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

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/dramc_register.h>
#include <soc/dramc_pi_api.h>
#include <soc/emi.h>
#include <soc/pll.h>
#include <symbols.h>

void mt_mem_init(const struct mt8173_sdram_params *sdram_params)
{
	int i = 0;

	/* init mempll */
	mem_pll_init(sdram_params);

	/* memory calibration */
	mt_set_emi(sdram_params);

	if (IS_ENABLED(CONFIG_MEMORY_TEST)) {
		/*
		 * do memory test:
		 * set memory scan range 0x2000
		 * larger test length, longer system boot up time
		 */
		i = complex_mem_test(_dram, 0x2000);

		printk(BIOS_DEBUG, "[MEM] complex R/W mem test %s : %d\n",
		       (i == 0) ? "pass" : "fail", i);

		ASSERT(i == 0);
	}
}
