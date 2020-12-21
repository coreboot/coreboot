/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <variant/gpio.h>

__weak void variant_configure_early_gpios(void) {}

void bootblock_mainboard_early_init(void)
{
	variant_configure_early_gpios();
}
