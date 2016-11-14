/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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
#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/sdram.h>
#include <string.h>
#include <types.h>

static const char *sdram_configs[] = {
#if IS_ENABLED(CONFIG_BOARD_GOOGLE_BOB)
	"sdram-lpddr3-samsung-2GB-24EB",
	"sdram-lpddr3-micron-2GB",
	"sdram-lpddr3-samsung-4GB-04EB",
	"sdram-lpddr3-micron-4GB",
#else
	"sdram-lpddr3-hynix-4GB-666",
	"sdram-lpddr3-hynix-4GB-800",
	"sdram-lpddr3-hynix-4GB-933",
#endif
};

static struct rk3399_sdram_params params;

#if IS_ENABLED(CONFIG_BOARD_GOOGLE_GRU) || \
    IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN)
enum dram_speeds {
	dram_666MHz = 0,
	dram_800MHz = 1,
	dram_933MHz = 2,
};

static enum dram_speeds get_sdram_index(void)
{
	uint32_t id;

	id = board_id();

	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN))
		switch (id) {
		case 4:
			return dram_800MHz;
		default:
			return dram_933MHz;
		}

	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_GRU))
			return dram_800MHz;
}
#endif

const struct rk3399_sdram_params *get_sdram_config()
{
#if IS_ENABLED(CONFIG_BOARD_GOOGLE_BOB)
	u32 ramcode = ram_code();

	/*
	 * through schematic, ramid arrange like following:
	 * 0: sdram-lpddr3-samsung-2GB-24EB
	 * 2: sdram-lpddr3-micron-2GB
	 * 4: sdram-lpddr3-samsung-4GB-04EB
	 * 6: sdram-lpddr3-micron-4GB
	 */
	ramcode = ramcode / 2;

	if (cbfs_boot_load_struct(sdram_configs[ramcode],
				  &params, sizeof(params)) != sizeof(params))
#else
	if (cbfs_boot_load_struct(sdram_configs[get_sdram_index()],
				  &params, sizeof(params)) != sizeof(params))
#endif
		die("Cannot load SDRAM parameter file!");
	return &params;
}
