/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_INTEL_USB4_PCIE_H__
#define __DRIVERS_INTEL_USB4_PCIE_H__

struct soc_intel_common_block_usb4_config {
	const char *desc;

	/* Pointer to USB4 device that this PCIe root port is routed to. */
	DEVTREE_CONST struct device *usb4_port;
};

#endif /* __DRIVERS_INTEL_USB4_PCIE_H__ */
