/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <variant/gpio.h>

void bootblock_mainboard_init(void)
{
	variant_configure_early_gpios();
}
