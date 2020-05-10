/* SPDX-License-Identifier: GPL-2.0-or-later  */

#include <soc/ramstage.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSP_SIL_UPD *params)
{
	/* Configure pads prior to SiliconInit() in case there's any
	 * dependencies during hardware initialization. */
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static void mainboard_enable(struct device *dev)
{
	/* Route 0x4e/4f to LPC */
	lpc_enable_fixed_io_ranges(LPC_IOE_EC_4E_4F);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
