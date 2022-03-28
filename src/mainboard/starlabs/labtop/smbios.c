/* SPDX-License-Identifier: GPL-2.0-only */

#include <chip.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <ec/starlabs/merlin/ec.h>
#include <smbios.h>
#include <types.h>
#include <uuid.h>
#include <variants.h>

const char *smbios_mainboard_bios_version(void)
{
	return "8";
}

/* Get the Embedded Controller firmware version */
void smbios_ec_revision(uint8_t *ec_major_revision, uint8_t *ec_minor_revision)
{
	u16 ec_version = it_get_version();

	*ec_major_revision = ec_version >> 8;
	*ec_minor_revision = ec_version & 0xff;
}

const char *smbios_system_sku(void)
{
	return CONFIG_MAINBOARD_FAMILY;
}

u8 smbios_mainboard_feature_flags(void)
{
	return SMBIOS_FEATURE_FLAGS_HOSTING_BOARD | SMBIOS_FEATURE_FLAGS_REPLACEABLE;
}

const char *smbios_chassis_version(void)
{
	return smbios_mainboard_version();
}

const char *smbios_chassis_serial_number(void)
{
	return smbios_mainboard_serial_number();
}
