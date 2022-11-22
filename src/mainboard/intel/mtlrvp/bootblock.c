/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <bootblock_common.h>

void bootblock_mainboard_early_init(void)
{
	configure_early_gpio_pads();
}
