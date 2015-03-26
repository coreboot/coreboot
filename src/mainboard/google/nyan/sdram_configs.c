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

#include <console/console.h>
#include <soc/sdram.h>

#include "sdram_configs.h"

static struct sdram_params sdram_configs[] = {
#include "bct/sdram-hynix-2GB-924.inc"			/* ram_code = 0000 */
#include "bct/sdram-hynix-4GB-792.inc"			/* ram_code = 0001 */
#include "bct/sdram-unused.inc"				/* ram_code = 0010 */
#include "bct/sdram-unused.inc"				/* ram_code = 0011 */
#include "bct/sdram-unused.inc"				/* ram_code = 0100 */
#include "bct/sdram-unused.inc"				/* ram_code = 0101 */
#include "bct/sdram-unused.inc"				/* ram_code = 0110 */
#include "bct/sdram-unused.inc"				/* ram_code = 0111 */
#include "bct/sdram-unused.inc"				/* ram_code = 1000 */
#include "bct/sdram-unused.inc"				/* ram_code = 1001 */
#include "bct/sdram-unused.inc"				/* ram_code = 1010 */
#include "bct/sdram-unused.inc"				/* ram_code = 1011 */
#include "bct/sdram-unused.inc"				/* ram_code = 1100 */
#include "bct/sdram-unused.inc"				/* ram_code = 1101 */
#include "bct/sdram-unused.inc"				/* ram_code = 1110 */
#include "bct/sdram-unused.inc"				/* ram_code = 1111 */
};

const struct sdram_params *get_sdram_config()
{
	uint32_t ramcode = sdram_get_ram_code();
	/*
	 * If we need to apply some special hacks to RAMCODE mapping (ex, by
	 * board_id), do that now.
	 */

	printk(BIOS_SPEW, "%s: RAMCODE=%d\n", __func__, ramcode);
	if (ramcode >= sizeof(sdram_configs) / sizeof(sdram_configs[0]) ||
	    sdram_configs[ramcode].MemoryType == NvBootMemoryType_Unused) {
		die("Invalid RAMCODE.");
	}

	return &sdram_configs[ramcode];
}
