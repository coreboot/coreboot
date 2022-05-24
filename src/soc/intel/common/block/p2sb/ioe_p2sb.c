/* SPDX-License-Identifier: GPL-2.0-or-later */

#define __SIMPLE_DEVICE__

#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/p2sblib.h>
#include <intelblocks/pcr.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

uint32_t ioe_p2sb_sbi_read(uint8_t pid, uint16_t reg)
{
	return p2sb_dev_sbi_read(PCI_DEV_IOE_P2SB, pid, reg);
}

void ioe_p2sb_sbi_write(uint8_t pid, uint16_t reg, uint32_t val)
{
	p2sb_dev_sbi_write(PCI_DEV_IOE_P2SB, pid, reg, val);
}

void ioe_p2sb_enable_bar(void)
{
	p2sb_dev_enable_bar(PCI_DEV_IOE_P2SB, IOE_P2SB_BAR);
}

static void read_resources(struct device *dev)
{
	mmio_resource_kb(dev, 0, IOE_P2SB_BAR / KiB, IOE_P2SB_SIZE / KiB);
}

struct device_operations device_ops = {
	.read_resources   = read_resources,
	.set_resources    = noop_set_resources,
	.ops_pci          = &pci_dev_ops_pci,
};

static const unsigned short pci_device_ids[] = {
	PCI_DID_INTEL_MTL_IOE_M_P2SB,
	PCI_DID_INTEL_MTL_IOE_P_P2SB,
	0,
};

static const struct pci_driver ioe_p2sb __pci_driver = {
	.ops     = &device_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = pci_device_ids,
};
