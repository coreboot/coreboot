/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>

static struct acpi_gpio lte_reset_gpio = ACPI_GPIO_OUTPUT_ACTIVE_LOW(GPP_H0);
static struct acpi_gpio lte_enable_gpio = ACPI_GPIO_OUTPUT_ACTIVE_HIGH(GPP_A10);
/* New lte reset for drapwer DVT*/
static struct acpi_gpio lte_new_reset_gpio =  ACPI_GPIO_OUTPUT_ACTIVE_LOW(GPP_H17);

void variant_devtree_update(void)
{

	uint32_t board_version = board_id();

	if (board_version <= 9) /* board version 9 is drawper EVT */
		update_lte_device(&lte_reset_gpio, &lte_enable_gpio);
	else
		update_lte_device(&lte_new_reset_gpio, &lte_enable_gpio);
}
