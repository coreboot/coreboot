/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <console/console.h>
#include <ec/smsc/mec1308/ec.h>
#include "ec.h"

void lumpy_ec_init(void)
{
	printk(BIOS_DEBUG, "%s\n", __func__);

	if (acpi_is_wakeup_s3())
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
