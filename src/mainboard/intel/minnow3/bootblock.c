/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bootblock_common.h>
#include <intelblocks/lpc_lib.h>
#include "gpio.h"

void bootblock_mainboard_init(void)
{
	const struct pad_config *pads;
	size_t num;

	lpc_configure_pads();

	/* Configure GPIOs needed prior to ramstage. */
	pads = early_gpio_table(&num);
	gpio_configure_pads(pads, num);
}
