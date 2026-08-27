/* SPDX-License-Identifier: BSD-3-Clause */

#include <smbios.h>
#include <console/console.h>

#include "vpd.h"

#define VPD_KEY_SYSTEM_FAMILY           "system_family"
#define VPD_KEY_SYSTEM_PRODUCT_NAME     "system_product_name"
#define VPD_KEY_SYSTEM_SKU              "system_sku"

const char *smbios_system_product_name(void)
{
	static char buffer[CONFIG_SMBIOS_SYSTEM_DATA_SIZE];
	if (vpd_gets(VPD_KEY_SYSTEM_PRODUCT_NAME, buffer, CONFIG_SMBIOS_SYSTEM_DATA_SIZE, VPD_RO))
		return buffer;

	printk(BIOS_WARNING, "DMI: Cannot read system_product_name from VPD, using smbios_mainboard_product_name() instead.\n");
	return smbios_mainboard_product_name();
}

const char *smbios_system_sku(void)
{
	static char buffer[CONFIG_SMBIOS_SYSTEM_DATA_SIZE];
	if (vpd_gets(VPD_KEY_SYSTEM_SKU, buffer, CONFIG_SMBIOS_SYSTEM_DATA_SIZE, VPD_RO))
		return buffer;

	printk(BIOS_WARNING, "DMI: Cannot read system_sku from VPD.\n");
	return "";
}

const char *smbios_system_family(void)
{
	static char buffer[CONFIG_SMBIOS_SYSTEM_DATA_SIZE];
	if (vpd_gets(VPD_KEY_SYSTEM_FAMILY, buffer, CONFIG_SMBIOS_SYSTEM_DATA_SIZE, VPD_RO))
		return buffer;

#ifdef CONFIG_MAINBOARD_FAMILY
	printk(BIOS_WARNING, "DMI: Cannot read system_family from VPD, using CONFIG_MAINBOARD_FAMILY instead.\n");
	return CONFIG_MAINBOARD_FAMILY;
#else
	printk(BIOS_WARNING, "DMI: Cannot read system_family from VPD.\n");
	return "";
#endif
}
