/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_device.h>

struct drivers_i2c_rt5645_config {
	const char *desc;	/* Device Description */
	const char *name;	/* ACPI Device Name */
	const char *hid;	/* ACPI _HID */
	struct acpi_gpio cbj_sleeve;	/* sleeve power gate GPIO */
	struct acpi_gpio hp_detect;
	unsigned int uid;
	unsigned int bus_speed;
	/* Allow GPIO based interrupt or PIRQ */
	struct acpi_gpio irq_gpio;
	struct acpi_irq irq;
	uint32_t jd_mode;
};
