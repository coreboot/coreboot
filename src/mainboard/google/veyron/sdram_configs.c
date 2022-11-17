/* SPDX-License-Identifier: GPL-2.0-only */
#include <boardid.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/sdram.h>
#include <types.h>

static struct rk3288_sdram_params sdram_configs[] = {
#include "sdram_inf/sdram-lpddr3-samsung-2GB.inc"	/* ram_code = 0000 */
#include "sdram_inf/sdram-lpddr3-hynix-2GB.inc"		/* ram_code = 0001 */
#include "sdram_inf/sdram-lpddr3-samsung-4GB-04EB.inc"	/* ram_code = 0010 */
#include "sdram_inf/sdram-lpddr3-hynix-2GB-2ranks.inc"	/* ram_code = 0011 */
#include "sdram_inf/sdram-ddr3-samsung-2GB.inc"		/* ram_code = 0100 */
#include "sdram_inf/sdram-ddr3-hynix-2GB.inc"		/* ram_code = 0101 */
#include "sdram_inf/sdram-ddr3-samsung-2GB.inc"		/* ram_code = 0110 */
#include "sdram_inf/sdram-lpddr3-elpida-2GB.inc"	/* ram_code = 0111 */
#include "sdram_inf/sdram-lpddr3-samsung-4GB.inc"	/* ram_code = 1000 */
#include "sdram_inf/sdram-lpddr3-hynix-4GB.inc"		/* ram_code = 1001 */
#include "sdram_inf/sdram-ddr3-nanya-2GB.inc"		/* ram_code = 1010 */
#include "sdram_inf/sdram-lpddr3-elpida-4GB.inc"	/* ram_code = 1011 */
#include "sdram_inf/sdram-lpddr3-samsung-2GB-24EB.inc"	/* ram_code = 1100 */
#include "sdram_inf/sdram-ddr3-hynix-2GB.inc"		/* ram_code = 1101 */
#include "sdram_inf/sdram-ddr3-samsung-4GB.inc"		/* ram_code = 1110 */
#include "sdram_inf/sdram-ddr3-hynix-4GB.inc"		/* ram_code = 1111 */
#include "sdram_inf/sdram-ddr3-samsung-2GB.inc"		/* ram_code = 000Z */
#include "sdram_inf/sdram-lpddr3-micron-2GB-D2.inc"	/* ram_code = 001Z */
#include "sdram_inf/sdram-lpddr3-hynix-2GB-BK.inc"	/* ram_code = 00Z0 */
#include "sdram_inf/sdram-lpddr3-hynix-2GB-BK.inc"	/* ram_code = 00Z1 */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 00ZZ */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 010Z */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 011Z */
#include "sdram_inf/sdram-unused.inc"			/* ram_code = 01Z0 */
};

_Static_assert(ARRAY_SIZE(sdram_configs) == 24, "Must have 24 sdram_configs!");

const struct rk3288_sdram_params *get_sdram_config(void)
{
	u32 ramcode = ram_code();

	if (ramcode >= ARRAY_SIZE(sdram_configs)
			|| sdram_configs[ramcode].dramtype == UNUSED)
		die("Invalid RAMCODE.");
	return &sdram_configs[ramcode];
}
