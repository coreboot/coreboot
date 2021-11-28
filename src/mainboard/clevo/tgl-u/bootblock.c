/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <ec/clevo/it5570e/early_init.h>
#include <intelblocks/lpc_lib.h>
#include <variant/gpio.h>

void bootblock_mainboard_early_init(void)
{
	variant_configure_early_gpios();

	/* Open LGMR window for EC */
	if (CONFIG(EC_CLEVO_IT5570E))
		lpc_open_mmio_window(CONFIG_EC_CLEVO_IT5570E_MEM_BASE, 64 * KiB);
}

void bootblock_mainboard_init(void)
{
	if (CONFIG(EC_CLEVO_IT5570E))
		ec_configure_kbled_booteffect();
}
