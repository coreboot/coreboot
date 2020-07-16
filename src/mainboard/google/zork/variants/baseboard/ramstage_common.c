/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_device.h>
#include <baseboard/variants.h>
#include <drivers/amd/i2s_machine_dev/chip.h>
#include <ec/google/chromeec/ec.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>

extern struct chip_operations drivers_amd_i2s_machine_dev_ops;

void variant_audio_update(void)
{
	const struct device *gpp_a_dev;
	const struct device *acp_dev;
	struct device *machine_dev = NULL;

	if (variant_uses_v3_schematics())
		return;

	gpp_a_dev = pcidev_path_on_root(PCIE_GPP_A_DEVFN);
	if (gpp_a_dev == NULL)
		return;

	acp_dev = pcidev_path_behind(gpp_a_dev->link_list, AUDIO_DEVFN);
	if (acp_dev == NULL)
		return;

	while ((machine_dev = dev_bus_each_child(acp_dev->link_list, machine_dev)) != NULL) {
		struct drivers_amd_i2s_machine_dev_config *cfg;
		struct acpi_gpio *gpio;

		if (machine_dev->chip_info == NULL)
			continue;

		if (machine_dev->chip_ops != &drivers_amd_i2s_machine_dev_ops)
			continue;

		cfg = machine_dev->chip_info;
		gpio = &cfg->dmic_select_gpio;

		if (CONFIG(BOARD_GOOGLE_BASEBOARD_TREMBYLE))
			gpio->pins[0] = GPIO_13;
		else
			gpio->pins[0] = GPIO_6;

		break;
	}
}
