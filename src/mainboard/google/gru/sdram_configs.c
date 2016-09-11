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
	"sdram-lpddr3-hynix-4GB-200",
	"sdram-lpddr3-hynix-4GB-666",
	"sdram-lpddr3-hynix-4GB-800",
	"sdram-lpddr3-hynix-4GB-666-no-odt",
	"sdram-lpddr3-hynix-4GB-800-no-odt",
	"sdram-lpddr3-hynix-4GB-933",
};

static struct rk3399_sdram_params params;

enum dram_speeds {
	dram_200MHz = 0,
	dram_666MHz = 1,
	dram_800MHz = 2,
	dram_666MHz_NO_ODT = 3,
	dram_800MHz_NO_ODT = 4,
	dram_933MHz = 5,
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
		switch (id) {
		case 0:
			return dram_800MHz_NO_ODT;
		default:
			return dram_800MHz;
		}
}

const struct rk3399_sdram_params *get_sdram_config()
{
	if (cbfs_boot_load_struct(sdram_configs[get_sdram_index()],
				  &params, sizeof(params)) != sizeof(params))
		die("Cannot load SDRAM parameter file!");
	return &params;
}
