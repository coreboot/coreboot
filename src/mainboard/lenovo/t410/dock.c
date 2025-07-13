/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <gpio.h>
#include "dock.h"
#include <southbridge/intel/common/gpio.h>
#include <ec/lenovo/h8/h8.h>
#include <ec/acpi/ec.h>

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
	/* UNTESTED */
	ec_set_bit(0x02, 0);
	ec_set_bit(0x1a, 0);
	ec_set_bit(0xfe, 4);
}

void dock_disconnect(void)
{
	/* UNTESTED */
	ec_clr_bit(0x02, 0);
	ec_clr_bit(0x1a, 0);
	ec_clr_bit(0xfe, 4);
}

int dock_present(void)
{
	const gpio_t gpio_num_array[] = {3, 4, 5};
	return gpio_base2_value(gpio_num_array, ARRAY_SIZE(gpio_num_array)) != 7;
}
