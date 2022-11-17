/* SPDX-License-Identifier: GPL-2.0-only */

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
