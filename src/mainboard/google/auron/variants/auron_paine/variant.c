/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <smbios.h>
#include <variant/onboard.h>
#include <mainboard/google/auron/variant.h>

int variant_smbios_data(device_t dev, int *handle,
				 unsigned long *current)
{
	int len = 0;

	len += smbios_write_type41(
		current, handle,
		BOARD_TRACKPAD_NAME,		/* name */
		BOARD_TRACKPAD_IRQ,		/* instance */
		BOARD_TRACKPAD_I2C_BUS,		/* segment */
		BOARD_TRACKPAD_I2C_ADDR,	/* bus */
		0,				/* device */
		0);				/* function */

	return len;
}
