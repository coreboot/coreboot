/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/p2sblib.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

uint32_t p2sb2_sbi_read(uint8_t pid, uint16_t reg)
{
	return p2sb_dev_sbi_read(PCI_DEV_P2SB2, pid, reg);
}

void p2sb2_sbi_write(uint8_t pid, uint16_t reg, uint32_t val)
{
	p2sb_dev_sbi_write(PCI_DEV_P2SB2, pid, reg, val);
}

void p2sb2_enable_bar(void)
{
	p2sb_dev_enable_bar(PCI_DEV_P2SB2, P2SB2_BAR);
}

static void read_resources(struct device *dev)
{
	mmio_range(dev, PCI_BASE_ADDRESS_0, P2SB2_BAR, P2SB2_SIZE);
}

struct device_operations p2sb2_ops = {
	.read_resources   = read_resources,
	.set_resources    = noop_set_resources,
	.ops_pci          = &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_WCL_P2SB2,
	PCI_DID_INTEL_PTL_H_P2SB2,
	PCI_DID_INTEL_PTL_U_H_P2SB2,
	0,
};

static const struct pci_driver p2sb2 __pci_driver = {
	.ops     = &p2sb2_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
