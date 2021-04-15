/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_AMD_I2S_MACHINE_DEV_H__
#define __DRIVERS_AMD_I2S_MACHINE_DEV_H__

#include <acpi/acpi_device.h>

struct drivers_amd_i2s_machine_dev_config {
	 /* ACPI _HID (required) */
	const char *hid;

	/* ACPI _UID */
	unsigned int uid;

	/*
	 * DMIC select GPIO (optional). Needs to be configured if the audio framework cannot use
	 * all the mics and select the right channel based on the use-case.
	 */
	struct acpi_gpio dmic_select_gpio;
};

#endif /* ___DRIVERS_AMD_I2S_MACHINE_DEV_H__ */
