/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>

/*
 * CIO2 is a PCI endpoint by default (no scan_bus). Boards that nest static
 * children under it (e.g. drivers/intel/mipi_camera) need a generic bus scan.
 */
struct device_operations cio_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.scan_bus		= scan_generic_bus,
	.ops_pci		= &pci_dev_ops_pci,
};
