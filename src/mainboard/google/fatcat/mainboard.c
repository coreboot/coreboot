/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/ec.h>
#include <soc/ramstage.h>
#include <stdio.h>
#include <stdlib.h>
#include <vendorcode/google/chromeos/chromeos.h>

void __weak fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	/* default implementation does nothing */
}

void mainboard_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	variant_update_soc_chip_config(config);
}

__weak void variant_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	/* default implementation does nothing */
}

static void mainboard_init(void *chip_info)
{
	struct pad_config *padbased_table;
	const struct pad_config *base_pads;
	size_t base_num;

	padbased_table = new_padbased_table();
	base_pads = variant_gpio_table(&base_num);
	gpio_padbased_override(padbased_table, base_pads, base_num);
	fw_config_gpio_padbased_override(padbased_table);
	gpio_configure_pads_with_padbased(padbased_table);
	free(padbased_table);
	baseboard_devtree_update();
}

void __weak baseboard_devtree_update(void)
{
	/* Override dev tree settings per baseboard */
}

void __weak variant_generate_s0ix_hook(enum s0ix_entry entry)
{
	/* Add board-specific MS0X entries */
	/*
	   if (s0ix_entry == S0IX_ENTRY) {
		implement variant operations here
	   }
	   if (s0ix_entry == S0IX_EXIT) {
		implement variant operations here
	   }
	 */
}

static void mainboard_dev_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_dev_init;
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
