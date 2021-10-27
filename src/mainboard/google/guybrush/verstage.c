/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/acpimmio.h>
#include <amdblocks/gpio.h>
#include <arch/io.h>
#include <baseboard/variants.h>
#include <security/vboot/vboot_common.h>
#include <soc/southbridge.h>

static void setup_gpio(void)
{
	const struct soc_amd_gpio *gpios, *override_gpios;
	size_t num_gpios, override_num_gpios;

	if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		gpios = variant_early_gpio_table(&num_gpios);
		override_gpios = variant_early_override_gpio_table(&override_num_gpios);

		gpio_configure_pads_with_override(gpios, num_gpios,
				override_gpios, override_num_gpios);
	}
}

void verstage_mainboard_early_init(void)
{
	setup_gpio();

	/*
	 * TODO : Make common function in cezanne code and just call it
	 * when PCI access is fixed in the PSP (b/186602472).
	 * For now the PSP doesn't configure LPC so it should be fine.
	 */
	if (CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)) {
		uint32_t dword;
		printk(BIOS_DEBUG, "Verstage configure eSPI\n");
		dword = pm_io_read32(PM_SPI_PAD_PU_PD);
		dword |= PM_ESPI_CS_USE_DATA2;
		pm_io_write32(PM_SPI_PAD_PU_PD, dword);

		dword = pm_io_read32(PM_ACPI_CONF);
		dword |= PM_ACPI_S5_LPC_PIN_MODE | PM_ACPI_S5_LPC_PIN_MODE_SEL;
		pm_io_write32(PM_ACPI_CONF, dword);
	}
}
