/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/i2c/tpm/chip.h>
#include <drivers/uart/acpi/chip.h>
#include <gpio.h>

static void cr50_devtree_update(void)
{
	const struct device *cr50_dev = DEV_PTR(cr50);
	struct drivers_i2c_tpm_config *cfg;
	struct acpi_gpio cr50_irq_gpio = ACPI_GPIO_IRQ_EDGE_LOW(GPIO_3);

	cfg = config_of(cr50_dev);
	cfg->irq_gpio = cr50_irq_gpio;
}

static void fpmcu_devtree_update(void)
{
	const struct device *fpmcu_dev = DEV_PTR(fpmcu);
	struct drivers_uart_acpi_config *cfg;
	struct acpi_gpio fpmcu_enable_gpio = ACPI_GPIO_OUTPUT_ACTIVE_HIGH(GPIO_32);

	cfg = config_of(fpmcu_dev);
	cfg->enable_gpio = fpmcu_enable_gpio;
}

void variant_devtree_update(void)
{
	uint32_t board_ver = board_id();

	if (board_ver > 1)
		return;

	cr50_devtree_update();
	fpmcu_devtree_update();
}
