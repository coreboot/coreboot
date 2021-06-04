/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>

static struct acpi_gpio lte_reset_gpio = ACPI_GPIO_OUTPUT_ACTIVE_LOW(GPP_H0);
static struct acpi_gpio lte_enable_gpio = ACPI_GPIO_OUTPUT_ACTIVE_HIGH(GPP_A10);

void variant_devtree_update(void)
{
	update_lte_device(&lte_reset_gpio, &lte_enable_gpio);
}
