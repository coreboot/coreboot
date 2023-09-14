/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>
#include <acpi/acpigen_ps2_keybd.h>
#include "gpio.h"

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	/*
	 * Librem 11's PS/2 keyboard has only two keys - volume up and volume
	 * down.
	 */
	enum ps2_action_key ps2_action_keys[2] = {
		PS2_KEY_VOL_DOWN,
		PS2_KEY_VOL_UP
	};
	acpigen_ps2_keyboard_dsd("_SB.PCI0.PS2K", ARRAY_SIZE(ps2_action_keys),
		ps2_action_keys, false, false, false, false);
}

static void mainboard_init(void *chip_info)
{
	gpio_configure_pads(gpio_table, ARRAY_SIZE(gpio_table));
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
