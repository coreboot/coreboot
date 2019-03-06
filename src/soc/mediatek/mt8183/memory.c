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

#include <assert.h>
#include <console/console.h>
#include <soc/dramc_pi_api.h>
#include <soc/emi.h>
#include <symbols.h>

void mt_mem_init(const struct sdram_params *params)
{
	u64 rank_size[RANK_MAX];

	/* memory calibration */
	mt_set_emi(params);

	if (CONFIG(MEMORY_TEST)) {
		size_t r;
		u8 *addr = _dram;

		dramc_get_rank_size(rank_size);

		for (r = RANK_0; r < RANK_MAX; r++) {
			int i;

			if (rank_size[r] == 0)
				break;

			i = complex_mem_test(addr, 0x2000);

			printk(BIOS_DEBUG, "[MEM] complex R/W mem test %s : %d\n",
			       (i == 0) ? "pass" : "fail", i);

			ASSERT(i == 0);

			addr += rank_size[r];
		}
	}
}
