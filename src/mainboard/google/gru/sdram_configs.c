/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/sdram.h>
#include <string.h>
#include <types.h>

static const char *sdram_configs[] = {

	/* Samsung K4E6E304EB-EGCE */
	[0] = "sdram-lpddr3-generic-4GB",

	/* Hynix H9CCNNNBJTALAR */
	[1] = "sdram-lpddr3-generic-4GB",

	/* Samsung K4E8E324EB-EGCF */
	[3] = "sdram-lpddr3-generic-2GB",

	/* Micron MT52L256M32D1PF */
	[4] = "sdram-lpddr3-generic-2GB",

	/* Samsung K4E6E304EB-EGCE, duplicate to 0 */
	[5] = "sdram-lpddr3-generic-4GB",

	/* Micron MT52L512M32D2PF */
	[6] = "sdram-lpddr3-generic-4GB",
};

static struct rk3399_sdram_params params;

enum dram_speeds {
	dram_800MHz = 800,
	dram_928MHz = 928,
};

static enum dram_speeds get_sdram_target_mhz(void)
{
	if (CONFIG(BOARD_GOOGLE_BOB) && board_id() < 4)
		return dram_800MHz;

	return dram_928MHz;
}

const struct rk3399_sdram_params *get_sdram_config(void)
{
	char config_file[64];
	uint32_t ramcode;

	ramcode = ram_code();
	if (ramcode >= ARRAY_SIZE(sdram_configs) ||
	    !snprintf(config_file, sizeof(config_file), "%s-%d",
	    sdram_configs[ramcode], get_sdram_target_mhz()) ||
	    (cbfs_load(config_file, &params, sizeof(params)) != sizeof(params)))
		die("Cannot load SDRAM parameter file!");

	return &params;
}
