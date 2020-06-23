/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_pi_api.h>

size_t sdram_size(void)
{
	size_t dram_size = 0x100000000;

	return dram_size;
}

void mt_set_emi(const struct dramc_data *dparam)
{
	init_dram(dparam);
}
