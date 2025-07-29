/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <bootblock_common.h>
#include <intelblocks/lpc_lib.h>

void bootblock_mainboard_early_init(void)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_early_gpio_table(&num);
	gpio_configure_pads(pads, num);
}

void bootblock_mainboard_init(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_RANGE))
		lpc_open_mmio_window(CONFIG_EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_BASE,
			CONFIG_EC_GOOGLE_CHROMEEC_LPC_GENERIC_MEMORY_SIZE);
}
