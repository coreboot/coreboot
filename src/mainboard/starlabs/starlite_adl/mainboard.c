/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen_ps2_keybd.h>
#include <device/device.h>
#include <soc/ramstage.h>
#include <option.h>
#include <variants.h>

static void init_mainboard(void *chip_info)
{
	const struct pad_config *pads;
	size_t num;

	pads = variant_gpio_table(&num);
	gpio_configure_pads(pads, num);

	devtree_update();
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	enum ps2_action_key ps2_action_keys[2] = {
		PS2_KEY_VOL_DOWN,
		PS2_KEY_VOL_UP
	};
	acpigen_ps2_keyboard_dsd("_SB.PCI0.PS2K", ARRAY_SIZE(ps2_action_keys),
		ps2_action_keys, false, false, false, false, false);
}

static void enable_mainboard(struct device *dev)
{
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = init_mainboard,
	.enable_dev = enable_mainboard,
};
