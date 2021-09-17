/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <drivers/usb/acpi/chip.h>

void variant_devtree_update(void)
{
	uint32_t board_version = board_id();
	struct device *lte_usb2 = DEV_PTR(lte_usb2);
	struct drivers_usb_acpi_config *config;
	struct acpi_gpio lte_reset_gpio = ACPI_GPIO_OUTPUT_ACTIVE_LOW(GPP_H0);

	/* board version 9 is drawper EVT */
	if (board_version > 9)
		return;

	if (lte_usb2 == NULL)
		return;

	config = config_of(lte_usb2);
	config->reset_gpio = lte_reset_gpio;
}
