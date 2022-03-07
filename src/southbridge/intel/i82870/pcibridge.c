/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "82870.h"

static void p64h2_pcix_init(struct device *dev)
{
	u32 dword;
	u8 byte;

	/* The purpose of changes to HCCR, ACNF, and MTT is to speed
	 * up the PCI bus for cards having high speed transfers.
	 */
	dword = 0xc2040002;
	pci_write_config32(dev, HCCR, dword);
	dword = 0x0000c3bf;
	pci_write_config32(dev, ACNF, dword);
	byte = 0x08;
	pci_write_config8(dev, MTT, byte);

}
static struct device_operations pcix_ops  = {
	.read_resources   = pci_bus_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_bus_enable_resources,
	.init             = p64h2_pcix_init,
	.scan_bus         = pci_scan_bridge,
	.reset_bus        = pci_bus_reset,
};

static const struct pci_driver pcix_driver __pci_driver = {
	.ops    = &pcix_ops,
	.vendor = PCI_VID_INTEL,
	.device = PCI_DID_INTEL_82870_1F0,
};

struct chip_operations southbridge_intel_i82870_ops = {
	CHIP_NAME("Intel 82870")
};
