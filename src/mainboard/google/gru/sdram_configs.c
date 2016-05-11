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
#if IS_ENABLED(CONFIG_BOARD_GOOGLE_GRU)
#include "sdram_inf/gru-sdram-lpddr3-hynix-4GB.inc"
#elif IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN)
#include "sdram_inf/kevin-sdram-lpddr3-hynix-4GB.inc"
#else
#error "What is your board name?"
#endif
};

const struct rk3399_sdram_params *get_sdram_config()
{
	u32 ramcode = ram_code();

	if (ramcode >= ARRAY_SIZE(sdram_configs)
			|| sdram_configs[ramcode].dramtype == UNUSED)
		die("Invalid RAMCODE.");
	return &sdram_configs[ramcode];
}
