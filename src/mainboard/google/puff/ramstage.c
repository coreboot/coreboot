/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <ec/ec.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>
#include <variant/gpio.h>

void mainboard_silicon_init_params(FSPS_UPD *supd)
{
	variant_devtree_update();
}

void __weak variant_devtree_update(void)
{
	/* Override dev tree settings per board */
}

void __weak variant_ramstage_init(void)
{
	/* Default weak implementation */
}

void __weak variant_mainboard_enable(struct device *dev)
{
	/* Override mainboard settings per board */
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	variant_mainboard_enable(dev);
}

static void mainboard_chip_init(void *chip_info)
{
	const struct pad_config *base_table;
	const struct pad_config *override_table;
	size_t base_gpios;
	size_t override_gpios;

	base_table = base_gpio_table(&base_gpios);
	override_table = override_gpio_table(&override_gpios);

	gpio_configure_pads_with_override(base_table,
					base_gpios,
					override_table,
					override_gpios);

	variant_ramstage_init();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
};
