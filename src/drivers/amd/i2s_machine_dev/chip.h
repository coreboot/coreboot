/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_AMD_I2S_MACHINE_DEV_H__
#define __DRIVERS_AMD_I2S_MACHINE_DEV_H__

#include <acpi/acpi_device.h>

struct drivers_amd_i2s_machine_dev_config {
	 /* ACPI _HID (required) */
	const char *hid;

	/* ACPI _UID */
	unsigned int uid;

	/* DMIC select GPIO (required) */
	struct acpi_gpio dmic_select_gpio;
};

#endif /* ___DRIVERS_AMD_I2S_MACHINE_DEV_H__ */
