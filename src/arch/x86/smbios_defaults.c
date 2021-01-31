/* SPDX-License-Identifier: GPL-2.0-only */

#include <memory_info.h>
#include <smbios.h>
#include <stdint.h>
#include <string.h>

/* this function will fill the corresponding locator */
__weak void smbios_fill_dimm_locator(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	char locator[40];

	snprintf(locator, sizeof(locator), "Channel-%d-DIMM-%d",
		dimm->channel_num, dimm->dimm_num);
	t->device_locator = smbios_add_string(t->eos, locator);

	snprintf(locator, sizeof(locator), "BANK %d", dimm->bank_locator);
	t->bank_locator = smbios_add_string(t->eos, locator);
}

__weak void smbios_fill_dimm_asset_tag(const struct dimm_info *dimm, struct smbios_type17 *t)
{
	char buf[40];

	snprintf(buf, sizeof(buf), "Channel-%d-DIMM-%d-AssetTag",
		dimm->channel_num, dimm->dimm_num);
	t->asset_tag = smbios_add_string(t->eos, buf);
}

__weak const char *smbios_mainboard_bios_version(void)
{
	return NULL;
}

__weak const char *smbios_mainboard_serial_number(void)
{
	return CONFIG_MAINBOARD_SERIAL_NUMBER;
}

__weak const char *smbios_mainboard_version(void)
{
	return CONFIG_MAINBOARD_VERSION;
}

__weak const char *smbios_mainboard_manufacturer(void)
{
	return CONFIG_MAINBOARD_SMBIOS_MANUFACTURER;
}

__weak const char *smbios_mainboard_product_name(void)
{
	return CONFIG_MAINBOARD_SMBIOS_PRODUCT_NAME;
}

__weak const char *smbios_mainboard_asset_tag(void)
{
	return "";
}

__weak u8 smbios_mainboard_feature_flags(void)
{
	return 0;
}

__weak const char *smbios_mainboard_location_in_chassis(void)
{
	return "";
}

__weak smbios_board_type smbios_mainboard_board_type(void)
{
	return SMBIOS_BOARD_TYPE_MOTHERBOARD;
}

__weak void smbios_ec_revision(uint8_t *ec_major_revision, uint8_t *ec_minor_revision)
{
	*ec_major_revision = 0x0;
	*ec_minor_revision = 0x0;
}

/*
 * System Enclosure or Chassis Types as defined in SMBIOS specification.
 * The default value is SMBIOS_ENCLOSURE_DESKTOP (0x03) but laptop,
 * convertible, or tablet enclosure will be used if the appropriate
 * system type is selected.
 */
__weak smbios_enclosure_type smbios_mainboard_enclosure_type(void)
{
	if (CONFIG(SYSTEM_TYPE_LAPTOP))
		return SMBIOS_ENCLOSURE_LAPTOP;
	else if (CONFIG(SYSTEM_TYPE_TABLET))
		return SMBIOS_ENCLOSURE_TABLET;
	else if (CONFIG(SYSTEM_TYPE_CONVERTIBLE))
		return SMBIOS_ENCLOSURE_CONVERTIBLE;
	else if (CONFIG(SYSTEM_TYPE_DETACHABLE))
		return SMBIOS_ENCLOSURE_DETACHABLE;
	else
		return SMBIOS_ENCLOSURE_DESKTOP;
}

__weak const char *smbios_system_serial_number(void)
{
	return smbios_mainboard_serial_number();
}

__weak const char *smbios_system_version(void)
{
	return smbios_mainboard_version();
}

__weak const char *smbios_system_manufacturer(void)
{
	return smbios_mainboard_manufacturer();
}

__weak const char *smbios_system_product_name(void)
{
	return smbios_mainboard_product_name();
}

__weak void smbios_system_set_uuid(u8 *uuid)
{
	/* leave all zero */
}

__weak unsigned int smbios_cpu_get_max_speed_mhz(void)
{
	return 0; /* Unknown */
}

__weak unsigned int smbios_cpu_get_current_speed_mhz(void)
{
	return 0; /* Unknown */
}

__weak const char *smbios_system_sku(void)
{
	return "";
}

__weak const char *smbios_chassis_version(void)
{
	return "";
}

__weak const char *smbios_chassis_serial_number(void)
{
	return "";
}

__weak const char *smbios_processor_serial_number(void)
{
	return "";
}

__weak u8 smbios_chassis_power_cords(void)
{
	return 1;
}
