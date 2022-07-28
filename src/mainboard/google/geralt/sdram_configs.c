/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dramc_param.h>

const struct sdram_info *get_sdram_config(void)
{
	/*
	 * The MT8188 platform supports "dram adaptive" feature to automatically
	 * detect dram information, including channel, rank, die size..., and
	 * can automatically configure EMI settings. Therefore, we will be
	 * passing a placeholder param blob.
	 */
	static struct sdram_info params;
	return &params;
}
