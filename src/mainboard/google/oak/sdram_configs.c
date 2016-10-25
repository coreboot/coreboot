/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <boardid.h>
#include <console/console.h>
#include <soc/emi.h>
#include <stdlib.h>

static const struct mt8173_sdram_params sdram_configs[] = {
#include "sdram_inf/sdram-lpddr3-H9CCNNN8GTMLAR-2GB.inc"  /* ram_code = 0000 */
#include "sdram_inf/sdram-lpddr3-K4E8E304EE-2GB.inc"      /* ram_code = 0001 */
#include "sdram_inf/sdram-lpddr3-K4E6E304EE-4GB.inc"      /* ram_code = 0010 */
#include "sdram_inf/sdram-lpddr3-H9CCNNNBLTBLAR-4GB.inc"  /* ram_code = 0011 */
#include "sdram_inf/sdram-lpddr3-K4E6E304EB-4GB.inc"      /* ram_code = 0100 */
#include "sdram_inf/sdram-unused.inc"                     /* ram_code = 0101 */
#include "sdram_inf/sdram-lpddr3-MT52L512M32D2PF-4GB.inc" /* ram_code = 0110 */
#include "sdram_inf/sdram-lpddr3-H9CCNNNBJTALAR-4GB.inc"  /* ram_code = 0111 */
#include "sdram_inf/sdram-lpddr3-MT52L256M32D1PF-2GB.inc" /* ram_code = 1000 */
#include "sdram_inf/sdram-unused.inc"                     /* ram_code = 1001 */
#include "sdram_inf/sdram-unused.inc"                     /* ram_code = 1010 */
#include "sdram_inf/sdram-unused.inc"                     /* ram_code = 1011 */
#include "sdram_inf/sdram-unused.inc"                     /* ram_code = 1100 */
#include "sdram_inf/sdram-unused.inc"                     /* ram_code = 1101 */
#include "sdram_inf/sdram-unused.inc"                     /* ram_code = 1110 */
#include "sdram_inf/sdram-unused.inc"                     /* ram_code = 1111 */
};

const struct mt8173_sdram_params *get_sdram_config(void)
{
	u32 ramcode = ram_code();

	if (ramcode >= ARRAY_SIZE(sdram_configs)
			|| sdram_configs[ramcode].type == TYPE_INVALID)
		die("Invalid RAMCODE.");

	return &sdram_configs[ramcode];
}
