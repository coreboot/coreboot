/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_INTEL_USB4_RETIMER_H__
#define __DRIVERS_INTEL_USB4_RETIMER_H__

#include <acpi/acpi_device.h>
#include <acpi/acpi.h>
#include <acpi/acpi_pld.h>

#define DFP_NUM_MAX	2

struct drivers_intel_usb4_retimer_config {
	/* Downstream facing port(DFP) */
	struct {
		/* GPIO used to control power of retimer device */
		struct acpi_gpio power_gpio;
		/* Type-C port associated with retimer */
		DEVTREE_CONST struct device *typec_port;
	} dfp[DFP_NUM_MAX];
};

#endif /* __DRIVERS_INTEL_USB4_RETIMER_H__ */
