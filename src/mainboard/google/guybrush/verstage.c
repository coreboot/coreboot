/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/acpimmio.h>
#include <arch/io.h>
#include <baseboard/variants.h>
#include <gpio.h>
#include <psp_verstage.h>
#include <security/vboot/vboot_common.h>
#include <soc/southbridge.h>

void verstage_mainboard_early_init(void)
{
	const struct soc_amd_gpio *gpios, *override_gpios;
	size_t num_gpios, override_num_gpios;

	gpios = variant_early_gpio_table(&num_gpios);
	override_gpios = variant_early_override_gpio_table(&override_num_gpios);
	gpio_configure_pads_with_override(gpios, num_gpios, override_gpios, override_num_gpios);
}

void verstage_mainboard_espi_init(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;
	uint32_t dword;

	gpios = variant_espi_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);

	dword = pm_io_read32(PM_SPI_PAD_PU_PD);
	dword |= PM_ESPI_CS_USE_DATA2;
	pm_io_write32(PM_SPI_PAD_PU_PD, dword);

	dword = pm_io_read32(PM_ACPI_CONF);
	dword |= PM_ACPI_S5_LPC_PIN_MODE | PM_ACPI_S5_LPC_PIN_MODE_SEL;
	pm_io_write32(PM_ACPI_CONF, dword);
}

void verstage_mainboard_tpm_init(void)
{
	const struct soc_amd_gpio *gpios;
	size_t num_gpios;

	gpios = variant_tpm_gpio_table(&num_gpios);
	gpio_configure_pads(gpios, num_gpios);
}
