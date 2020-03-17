/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <southbridge/intel/common/gpio.h>
#include <ec/lenovo/h8/h8.h>
#include <ec/acpi/ec.h>

#include "../../dock.h"

void init_dock(void)
{
	if (dock_present()) {
		printk(BIOS_DEBUG, "dock is connected\n");
		dock_connect();
	} else
		printk(BIOS_DEBUG, "dock is not connected\n");
}

void dock_connect(void)
{
	ec_set_bit(0x02, 0);
	set_gpio(28, GPIO_LEVEL_HIGH);
}

void dock_disconnect(void)
{
	ec_clr_bit(0x02, 0);
	set_gpio(28, GPIO_LEVEL_LOW);
}

int dock_present(void)
{
	const int dock_id_gpio[] = { 2, 3, 4, -1};

	return get_gpios(dock_id_gpio) != 7;
}
