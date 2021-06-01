/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>
#include <types.h>
#include <uuid.h>

#include <ec/starlabs/it8987/ec.h>

/* Override the BIOS version using smbios_mainboard_bios_version() */
const char *smbios_mainboard_bios_version(void)
{
	return "6";
}

/* Get the Embedded Controller firmware version */
void smbios_ec_revision(uint8_t *ec_major_revision, uint8_t *ec_minor_revision)
{
	u16 ec_version = it8987_get_version();

	*ec_major_revision = ec_version >> 8;
	*ec_minor_revision = ec_version & 0xff;
}

/* Override smbios_system_manufacturer */
const char *smbios_system_manufacturer(void)
{
	return "Star Labs";
}

/* Override smbios_system_sku */
const char *smbios_system_sku(void)
{
#if CONFIG(BOARD_STARLABS_LABTOP_CML)
	return "L4";
#else
	return "L3-U";
#endif
}

/* Override smbios_mainboard_features_flags */
u8 smbios_mainboard_feature_flags(void)
{
	return SMBIOS_FEATURE_FLAG_HOSTING_BOARD | SMBIOS_FEATURE_FLAG_REPLACEABLE;
}

/* Override smbios_mainboard_location_in_chassis */
const char *smbios_mainboard_location_in_chassis(void)
{
	return "Default";
}

/* Override smbios_mainboard_board_type */
smbios_board_type smbios_mainboard_board_type(void)
{
	return SMBIOS_BOARD_TYPE_MOTHERBOARD;
}

/* Override smbios_mainboard_asset_tag */
const char *smbios_mainboard_asset_tag(void)
{
	return "Default";
}

smbios_enclosure_type smbios_mainboard_enclosure_type(void)
{
	return SMBIOS_ENCLOSURE_NOTEBOOK;
}

/* Override smbios_chassis_version */
const char *smbios_chassis_version(void)
{
	return smbios_mainboard_version();
}

/* Override smbios_chassis_serial_number */
const char *smbios_chassis_serial_number(void)
{
	return smbios_mainboard_serial_number();
}

/* Override smbios_chassis_asset_tag */
const char *smbios_chassis_asset_tag(void)
{
	return CONFIG_MAINBOARD_SERIAL_NUMBER;
}
