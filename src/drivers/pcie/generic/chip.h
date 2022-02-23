/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PCIE_GENERIC_H_
#define _PCIE_GENERIC_H_

#include <types.h>

struct drivers_pcie_generic_config {
	bool is_untrusted;
	/*
	 * This needs to be pointed to the device instance in the device tree when
	 * there is already a device with the root port so that the ACPI code to be
	 * generated will be added to that existing device.
	 * By default, an ACPI device named 'DEV0' is created under the root port if
	 * this does not reference to a device.
	 */
	DEVTREE_CONST struct device *companion_dev;
};

#endif /* _PCIE_GENERIC_H_ */
