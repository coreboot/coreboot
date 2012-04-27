/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/acpi.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <types.h>
#include <console/console.h>
#include <ec/smsc/mec1308/ec.h>
#include "ec.h"

void lumpy_ec_init(void)
{
	printk(BIOS_DEBUG, "lumpy_ec_init\n");

	if (acpi_slp_type == 3)
		return;

	/*
	 * Enable EC control of fan speed.
	 *
	 * This will be changed to OS control in ACPI EC _REG
	 * method when the OS is ready to control the fan.
	 */
	ec_write(EC_FAN_SPEED, 0);

	send_ec_command_data(EC_BATTERY_MODE, EC_BATTERY_MODE_NORMAL);
	send_ec_command_data(EC_POWER_BUTTON_MODE, EC_POWER_BUTTON_MODE_OS);
	send_ec_command(EC_SMI_DISABLE);
	send_ec_command(EC_ACPI_ENABLE);
	send_ec_command(EC_BACKLIGHT_ON);
}
