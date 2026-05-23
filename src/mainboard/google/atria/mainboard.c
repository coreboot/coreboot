/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <baseboard/variants.h>
#include <baseboard/gpio.h>
#include <bootstate.h>
#include <device/device.h>
#include <variant/ec.h>

static void mainboard_init(void *chip_info)
{
	mainboard_ec_init();
}

static void mainboard_early(void *unused)
{
	struct pad_config *padbased_table;
	const struct pad_config *base_pads;
	size_t base_num;

	padbased_table = new_padbased_table();
	base_pads = variant_gpio_table(&base_num);
	gpio_padbased_override(padbased_table, base_pads, base_num);
	gpio_configure_pads_with_padbased(padbased_table);
	free(padbased_table);
}

BOOT_STATE_INIT_ENTRY(BS_PRE_DEVICE, BS_ON_EXIT, mainboard_early, NULL);

static void mainboard_fill_ssdt(const struct device *dev)
{
	/* TODO: Add mainboard specific SSDT */
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
