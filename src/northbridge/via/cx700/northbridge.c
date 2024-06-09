/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>

struct device_operations domain_ops = {
	.read_resources	  = pci_domain_read_resources,
	.set_resources	  = pci_domain_set_resources,
	.scan_bus	  = pci_host_bridge_scan_bus,
};
