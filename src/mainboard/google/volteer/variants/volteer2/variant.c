/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/pci_devs.h>
#include <acpi/acpi_device.h>

/*
 * This function runs only on the volteer_ti50 variant, which has the GSC on a
 * reworked I2C bus.
 */
static void devtree_enable_i2c_tpm(void)
{
	struct device *spi_tpm = DEV_PTR(spi_tpm);
	struct device *i2c_tpm = DEV_PTR(i2c_tpm);
	if (!i2c_tpm || !spi_tpm)
		return;
	spi_tpm->enabled = 0;
	i2c_tpm->enabled = 1;
}

void variant_devtree_update(void)
{
	if (CONFIG(I2C_TPM))
		devtree_enable_i2c_tpm();
}
