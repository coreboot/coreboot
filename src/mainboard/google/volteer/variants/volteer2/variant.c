/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/pci_devs.h>
#include <acpi/acpi_device.h>

extern struct chip_operations drivers_i2c_tpm_ops;

static bool match_i2c_tpm(DEVTREE_CONST struct device *dev)
{
	return dev->chip_ops == &drivers_i2c_tpm_ops;
}

/*
 * This function runs only on the volteer_ti50 variant, which has the GSC on a
 * reworked I2C bus.
 */
static void devtree_enable_i2c_tpm(void)
{
	struct device *spi_tpm = pcidev_path_on_root(PCH_DEVFN_GSPI0)->link_list->children;
	struct device *i2c_tpm = dev_find_matching_device_on_bus(
		pcidev_path_on_root(PCH_DEVFN_I2C1)->link_list, match_i2c_tpm);
	if (!i2c_tpm || !spi_tpm)
		return;
	spi_tpm->enabled = 0;
	i2c_tpm->enabled = 1;
}

void variant_devtree_update(void)
{
	if (CONFIG(MAINBOARD_HAS_I2C_TPM_CR50))
		devtree_enable_i2c_tpm();
}
