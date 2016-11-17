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
    [0] = "sdram-lpddr3-hynix-4GB",
    [3] = "sdram-lpddr3-samsung-2GB-24EB",
    [4] = "sdram-lpddr3-micron-2GB",
    [5] = "sdram-lpddr3-samsung-4GB-04EB",
    [6] = "sdram-lpddr3-micron-4GB",
};

static struct rk3399_sdram_params params;

const struct rk3399_sdram_params *get_sdram_config()
{
	uint32_t ramcode;

	ramcode = ram_code();
	if (ramcode >= ARRAY_SIZE(sdram_configs) || !sdram_configs[ramcode] ||
	    (cbfs_boot_load_struct(sdram_configs[ramcode],
				   &params, sizeof(params)) != sizeof(params)))
		die("Cannot load SDRAM parameter file!");
	return &params;
}
