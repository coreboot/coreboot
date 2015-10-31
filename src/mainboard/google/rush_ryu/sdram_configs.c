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

#include <console/console.h>
#include <soc/sdram_configs.h>
#include <stdlib.h>

static const struct sdram_params sdram_configs[] = {
#include "bct/sdram-samsung-4GB-924.inc"		/* ram_code = 00 */
#include "bct/sdram-hynix-4GB-924.inc"			/* ram_code = 01 */
#include "bct/sdram-micron-4GB-924.inc"			/* ram_code = 10 */
#include "bct/sdram-micron-4GB-528.inc"			/* ram_code = 11 */
};

const struct sdram_params *get_sdram_config()
{
	uint32_t ramcode = sdram_get_ram_code();

	/*
	 * If we need to apply some special hacks to RAMCODE mapping (ex, by
	 * board_id), do that now.
	 */

	ramcode &= 0x3;		/* Only bits 1:0 used on Ryu */
	printk(BIOS_SPEW, "%s: RAMCODE=%d\n", __func__, ramcode);

	if (ramcode >= ARRAY_SIZE(sdram_configs) ||
	    sdram_configs[ramcode].MemoryType == NvBootMemoryType_Unused) {
		die("Invalid RAMCODE.");
	}

	return &sdram_configs[ramcode];
}
