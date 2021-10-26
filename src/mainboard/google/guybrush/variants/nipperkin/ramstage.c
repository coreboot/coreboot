/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/i2c/tpm/chip.h>
#include <soc/gpio.h>

void variant_devtree_update(void)
{
	uint32_t board_ver = board_id();
	const struct device *cr50_dev = DEV_PTR(cr50);
	struct drivers_i2c_tpm_config *cfg;
	struct acpi_gpio cr50_irq_gpio = ACPI_GPIO_IRQ_EDGE_LOW(GPIO_3);

	if (board_ver > 1)
		return;

	cfg = config_of(cr50_dev);
	cfg->irq_gpio = cr50_irq_gpio;
}
