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
#include <console/console.h>
#include <gpio.h>
#include <soc/sdram.h>
#include <string.h>
#include <types.h>

static struct rk3399_sdram_params sdram_configs[] = {
#include "sdram_inf/sdram-lpddr3-hynix-4GB-200.inc"
/* THIS IS FOR KEVIN ONLY! crosbug.com/p/54144 */
#include "sdram_inf/kevin-sdram-lpddr3-hynix-4GB.inc"
#include "sdram_inf/sdram-lpddr3-hynix-4GB-666.inc"
#include "sdram_inf/sdram-lpddr3-hynix-4GB-800.inc"
/* #include "sdram_inf/sdram-lpddr3-hynix-4GB-928.inc" */
};

enum dram_speeds {
	dram_200MHz = 0,
	dram_300MHz = 1,
	dram_666MHz = 2,
	dram_800MHz = 3,
/*	dram_928MHz = 4, */
};

static enum dram_speeds get_sdram_index(void)
{
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN))
		return dram_300MHz;
	else if (IS_ENABLED(CONFIG_BOARD_GOOGLE_GRU))
		return dram_800MHz;
	else
		return dram_200MHz;
}

const struct rk3399_sdram_params *get_sdram_config()
{

	enum dram_speeds speed = get_sdram_index();

	printk(BIOS_INFO, "Using SDRAM configuration for %d MHz\n",
	       sdram_configs[speed].ddr_freq / (1000 * 1000));

	return &sdram_configs[speed];
}


uint32_t ram_code(void)
{
	return get_sdram_index();
}
