/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <drivers/intel/usb4/retimer/retimer.h>
#include <intelblocks/tcss.h>
#include <static.h>

int retimer_get_index_for_typec(uint8_t typec_port)
{
	const struct device *tcss_port_arr[] = {
		DEV_PTR(tcss_usb3_port0),
		DEV_PTR(tcss_usb3_port1),
#if CONFIG(SOC_INTEL_PANTHERLAKE)
		DEV_PTR(tcss_usb3_port2),
		DEV_PTR(tcss_usb3_port3),
#endif
	};

	for (int i = 0, ec_port = 0; i < MAX_TYPE_C_PORTS; i++) {
		if (i == typec_port)
			return ec_port;

		if (is_dev_enabled(tcss_port_arr[i]))
			ec_port++;
	}

	/* Code should not come here if typec_port input is correct */
	return -1;
}
