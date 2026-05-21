/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <intelblocks/lpc_lib.h>
#include <baseboard/variants.h>

void bootblock_mainboard_early_init(void)
{
	/* TODO: Perform mainboard initialization */
}

void bootblock_mainboard_init(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_RANGE))
		lpc_open_mmio_window(CONFIG_EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_BASE,
			CONFIG_EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_SIZE);

}
