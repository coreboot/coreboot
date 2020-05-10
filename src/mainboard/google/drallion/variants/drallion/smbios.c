/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>
#include <variant/variant.h>

smbios_enclosure_type smbios_mainboard_enclosure_type(void)
{
	return has_360_sensor_board() ?
	       SMBIOS_ENCLOSURE_CONVERTIBLE :
	       SMBIOS_ENCLOSURE_LAPTOP;
}
