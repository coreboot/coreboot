/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_INTEL_USB4_RETIMER_H__
#define __DRIVERS_INTEL_USB4_RETIMER_H__

#include <acpi/acpi_device.h>

struct drivers_intel_usb4_retimer_config {
	/* GPIO used to control power of retimer device. */
	struct acpi_gpio power_gpio;
};

#endif /* __DRIVERS_INTEL_USB4_RETIMER_H__ */
