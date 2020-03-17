/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 */

#include <smbios.h>
#include <variant/variant.h>

smbios_enclosure_type smbios_mainboard_enclosure_type(void)
{
	return has_360_sensor_board() ?
	       SMBIOS_ENCLOSURE_CONVERTIBLE :
	       SMBIOS_ENCLOSURE_LAPTOP;
}
