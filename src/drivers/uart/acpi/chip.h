/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_UART_ACPI_H__
#define __DRIVERS_UART_ACPI_H__

#include <acpi/acpi_device.h>

struct drivers_uart_acpi_config {
	const char *hid;		/* ACPI _HID (required) */
	const char *cid;		/* ACPI _CID */
	const char *name;		/* ACPI Device Name */
	const char *desc;		/* Device Description */
	unsigned int uid;		/* ACPI _UID */
	unsigned int wake;		/* Wake GPE */
	const char *compat_string;	/* Compatible string for _HID=PRP0001 */
	struct acpi_irq irq;		/* PIRQ */
	struct acpi_gpio irq_gpio;	/* GPIO to be used instead of PIRQ */
	struct acpi_uart uart;		/* UART device information */

	/* Does the device have a power resource? */
	bool has_power_resource;

	/* GPIO used to take device out of reset or to put it into reset. */
	struct acpi_gpio reset_gpio;
	/* Delay to be inserted after device is taken out of reset. */
	unsigned int reset_delay_ms;
	/* Delay to be inserted after device is put into reset. */
	unsigned int reset_off_delay_ms;

	/* GPIO used to enable device. */
	struct acpi_gpio enable_gpio;
	/* Delay to be inserted after device is enabled. */
	unsigned int enable_delay_ms;
	/* Delay to be inserted after device is disabled. */
	unsigned int enable_off_delay_ms;

	/* GPIO used to stop operation of device. */
	struct acpi_gpio stop_gpio;
	/* Delay to be inserted after disabling stop. */
	unsigned int stop_delay_ms;
	/* Delay to be inserted after enabling stop. */
	unsigned int stop_off_delay_ms;
};

#endif /* __DRIVERS_UART_ACPI_H__ */
