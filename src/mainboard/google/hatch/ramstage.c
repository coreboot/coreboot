/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <device/device.h>
#include <drivers/i2c/generic/chip.h>
#include <ec/ec.h>
#include <intelblocks/gpio.h>
#include <option.h>
#include <soc/gpio.h>
#include <soc/ramstage.h>
#include <static.h>
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

static void update_touchpad_wake_config(void)
{
	const struct device *touchpad = DEV_PTR(touchpad);
	if (!touchpad || !touchpad->chip_info)
		return;

	/* Check CFR option for touchpad wake */
	unsigned int touchpad_wake_enabled = get_uint_option("touchpad_wake", 0);

	printk(BIOS_DEBUG, "Touchpad wake: %s\n", touchpad_wake_enabled?"enabled":"disabled");

	/* Modify devicetree config directly */
	struct drivers_i2c_generic_config *config =
		(struct drivers_i2c_generic_config *)touchpad->chip_info;
	if (!touchpad_wake_enabled) {
		/* Disable wake GPE and wake flag in IRQ descriptor */
		config->wake = 0;
		config->irq.wake = 0;

		/* Reconfigure GPIO pad to remove wake capability */
		const struct pad_config touchpad_gpio[] = {
			PAD_CFG_GPI_APIC(GPP_A21, NONE, DEEP, LEVEL, INVERT),
		};
		gpio_configure_pads(touchpad_gpio, ARRAY_SIZE(touchpad_gpio));
	}
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	variant_mainboard_enable(dev);
	update_touchpad_wake_config();
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

void __weak variant_final(void)
{
}

static void mainboard_final(void *chip_info)
{
	const struct pad_config *variant_finalize;
	size_t variant_gpios;
	variant_finalize = variant_finalize_gpio_table(&variant_gpios);
	gpio_configure_pads(variant_finalize, variant_gpios);

	variant_final();
}

struct chip_operations mainboard_ops = {
	.init = mainboard_chip_init,
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};
