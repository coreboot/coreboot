/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/dramc_param.h>

static const char *const sdram_configs[] = {
	[0] = "sdram-lpddr4x-MT29VZZZBD9DQKPR-046-6GB",
	[1] = "sdram-lpddr4x-MT29VZZZAD8GQFSL-046-4GB",
};

static struct sdram_info params;

const struct sdram_info *get_sdram_config(void)
{
	uint32_t ramcode = ram_code();

	if (ramcode >= ARRAY_SIZE(sdram_configs) ||
	    cbfs_load(sdram_configs[ramcode], &params, sizeof(params)) != sizeof(params))
		die("Cannot load SDRAM parameter file for RAM code: %#x", ramcode);

	return &params;
}
