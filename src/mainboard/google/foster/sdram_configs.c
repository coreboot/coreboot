/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <soc/sdram_configs.h>

static struct sdram_params sdram_configs[] = {
#include "bct/sdram-samsung-3GB-204.inc"		/* ram_code = 0000 */
#include "bct/sdram-unused.inc"				/* ram_code = 0001 */
#include "bct/sdram-unused.inc"				/* ram_code = 0010 */
#include "bct/sdram-unused.inc"				/* ram_code = 0011 */
};

const struct sdram_params *get_sdram_config(void)
{
	uint32_t ramcode = sdram_get_ram_code();
	/*
	 * If we need to apply some special hacks to RAMCODE mapping (ex, by
	 * board_id), do that now.
	 */

	printk(BIOS_SPEW, "%s: RAMCODE=%d\n", __func__, ramcode);
	if (ramcode >= ARRAY_SIZE(sdram_configs) ||
	    sdram_configs[ramcode].MemoryType == NvBootMemoryType_Unused) {
		die("Invalid RAMCODE.");
	}

	return &sdram_configs[ramcode];
}
