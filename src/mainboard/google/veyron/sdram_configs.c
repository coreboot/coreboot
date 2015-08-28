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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */
#include <arch/io.h>
#include <boardid.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/sdram.h>
#include <string.h>
#include <types.h>

static struct rk3288_sdram_params sdram_configs[] = {
#include "sdram_inf/sdram-lpddr3-samsung-2GB.inc"	/* ram_code = 0000 */
#include "sdram_inf/sdram-lpddr3-hynix-2GB.inc"		/* ram_code = 0001 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 0010 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 0011 */
#include "sdram_inf/sdram-ddr3-samsung-2GB.inc"		/* ram_code = 0100 */
#include "sdram_inf/sdram-ddr3-hynix-2GB.inc"		/* ram_code = 0101 */
#include "sdram_inf/sdram-ddr3-samsung-2GB.inc"		/* ram_code = 0110 */
#include "sdram_inf/sdram-lpddr3-elpida-2GB.inc"	/* ram_code = 0111 */
#include "sdram_inf/sdram-lpddr3-samsung-4GB.inc"	/* ram_code = 1000 */
#include "sdram_inf/sdram-lpddr3-hynix-4GB.inc"		/* ram_code = 1001 */
#include "sdram_inf/sdram-ddr3-nanya-2GB.inc"		/* ram_code = 1010 */
#include "sdram_inf/sdram-lpddr3-elpida-4GB.inc"	/* ram_code = 1011 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 1100 */
#include "sdram_inf/sdram-ddr3-hynix-2GB.inc"		/* ram_code = 1101 */
#include "sdram_inf/sdram-ddr3-samsung-4GB.inc"		/* ram_code = 1110 */
#include "sdram_inf/sdram-ddr3-hynix-4GB.inc"		/* ram_code = 1111 */
};

const struct rk3288_sdram_params *get_sdram_config()
{
	u32 ramcode = ram_code();

	if (ramcode >= ARRAY_SIZE(sdram_configs)
			|| sdram_configs[ramcode].dramtype == UNUSED)
		die("Invalid RAMCODE.");
	return &sdram_configs[ramcode];
}
