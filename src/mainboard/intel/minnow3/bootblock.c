/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include "gpio.h"

void bootblock_mainboard_init(void)
{
	const struct pad_config *pads;
	size_t num;

	/* Configure GPIOs needed prior to ramstage. */
	pads = early_gpio_table(&num);
	gpio_configure_pads(pads, num);
}
