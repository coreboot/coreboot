/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DRIVERS_PCIE_RTD3_DEVICE_CHIP_H__
#define __DRIVERS_PCIE_RTD3_DEVICE_CHIP_H__

#include <acpi/acpi_device.h>

/* Adds a GPIO controlled ACPI PowerResource for a PCIe device. */
struct drivers_pcie_rtd3_device_config {
	/* Name of ACPI node for the device */
	const char *name;

	/* GPIO used to enable device. */
	struct acpi_gpio enable_gpio;
	/* Delay to be inserted after device is enabled. */
	unsigned int enable_delay_ms;
	/* Delay to be inserted after device is disabled. */
	unsigned int enable_off_delay_ms;

	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio reset_gpio;
	/* Delay to be inserted after device is taken out of reset. */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset. */
	unsigned int reset_off_delay_ms;
};

#endif /* __DRIVERS_PCIE_RTD3_DEVICE_CHIP_H__ */
