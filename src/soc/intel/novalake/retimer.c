/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <drivers/intel/usb4/retimer/retimer.h>
#include <intelblocks/tcss.h>
#include <static.h>

WEAK_DEV_PTR(tcss_usb3_port0);
WEAK_DEV_PTR(tcss_usb3_port1);
WEAK_DEV_PTR(tcss_usb3_port2);

int retimer_get_index_for_typec(uint8_t typec_port)
{
	const struct device *tcss_port_arr[] = {
		DEV_PTR(tcss_usb3_port0),
		DEV_PTR(tcss_usb3_port1),
#if CONFIG_MAX_TCSS_PORTS > 2
		DEV_PTR(tcss_usb3_port2),
#endif
#if CONFIG_MAX_TCSS_PORTS > 3
		DEV_PTR(tcss_usb3_port2),
#endif
	};

	int max_port = ARRAY_SIZE(tcss_port_arr);

	for (int i = 0, ec_port = 0; i < max_port; i++) {
		if (i == typec_port) {
			printk(BIOS_INFO, "USB Type-C %d mapped to EC port %d\n", typec_port,
			       ec_port);
			return ec_port;
		}

		if (is_dev_enabled(tcss_port_arr[i]))
			ec_port++;
	}

	/* Code should not come here if typec_port input is correct */
	return -1;
}
