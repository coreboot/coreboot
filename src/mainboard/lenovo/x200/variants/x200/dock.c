/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <gpio.h>
#include <southbridge/intel/common/gpio.h>
#include <ec/lenovo/h8/h8.h>
#include <ec/acpi/ec.h>

#include "../../dock.h"

void h8_mb_init(void)
{
	if (dock_present()) {
		printk(BIOS_DEBUG, "dock is connected\n");
		dock_connect();
	} else {
		printk(BIOS_DEBUG, "dock is not connected\n");
	}
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
	const gpio_t dock_id_gpio[] = {2, 3, 4};

	return gpio_base2_value(dock_id_gpio, ARRAY_SIZE(dock_id_gpio)) != 7;
}
