/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <drivers/intel/usb4/retimer/retimer.h>
#include <intelblocks/tcss.h>
#include <stdint.h>

int retimer_get_index_for_typec(uint8_t typec_port)
{
	int ec_port = 0;

	const struct device *tcss_port_arr[] = {
		DEV_PTR(tcss_usb3_port1),
		DEV_PTR(tcss_usb3_port2),
		DEV_PTR(tcss_usb3_port3),
		DEV_PTR(tcss_usb3_port4),
	};

	for (uint8_t i = 0; i < MAX_TYPE_C_PORTS; i++) {
		if (i == typec_port) {
			printk(BIOS_ERR, "USB Type-C %d mapped to EC port %d\n", typec_port,
					ec_port);
			return ec_port;
		}

		if (is_dev_enabled(tcss_port_arr[i]))
			ec_port++;
	}

	// Code should not come here if typec_port input is correct
	return -1;
}
