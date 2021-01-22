/* SPDX-License-Identifier: GPL-2.0-only */

#include <ec/google/chromeec/ec.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <cbfs.h>
#include <gpio.h>
#include <smbios.h>
#include <variant/gpio.h>

uint32_t sku_id(void)
{
	static int sku = -1;

	if (sku == -1)
		sku = google_chromeec_get_sku_id();

	return sku;
}

uint8_t variant_board_sku(void)
{
	return sku_id();
}

void variant_mainboard_suspend_resume(void)
{
	/* Enable backlight - GPIO 133 active low */
	gpio_set(GPIO_133, 0);
}

const char *smbios_mainboard_manufacturer(void)
{
	static char oem_bin_data[11];
	static const char *manuf;

	if (!CONFIG(USE_OEM_BIN))
		return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	if (manuf)
		return manuf;

	if (cbfs_load("oem.bin", oem_bin_data, sizeof(oem_bin_data) - 1))
		manuf = &oem_bin_data[0];
	else
		manuf = CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;

	return manuf;
}
