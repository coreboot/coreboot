/*
 * This file is part of the coreboot project.
 *
 * (C) 2004 Linux Networx
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/io.h>
#include <delay.h>
#include "amd8111.h"


#define CMD3		0x54

typedef enum {
	VAL3			= (1 << 31),   /* VAL bit for byte 3 */
	VAL2			= (1 << 23),   /* VAL bit for byte 2 */
	VAL1			= (1 << 15),   /* VAL bit for byte 1 */
	VAL0			= (1 << 7),    /* VAL bit for byte 0 */
}VAL_BITS;

typedef enum {
	/* VAL3 */
	ASF_INIT_DONE_ALIAS	= (1 << 29),
	/* VAL2 */
	JUMBO			= (1 << 21),
	VSIZE			= (1 << 20),
	VLONLY			= (1 << 19),
	VL_TAG_DEL		= (1 << 18),
	/* VAL1 */
	EN_PMGR			= (1 << 14),
	INTLEVEL		= (1 << 13),
	FORCE_FULL_DUPLEX	= (1 << 12),
	FORCE_LINK_STATUS	= (1 << 11),
	APEP			= (1 << 10),
	MPPLBA			= (1 << 9),
	/* VAL0 */
	RESET_PHY_PULSE		= (1 << 2),
	RESET_PHY		= (1 << 1),
	PHY_RST_POL		= (1 << 0),
}CMD3_BITS;

static void nic_init(struct device *dev)
{
	struct southbridge_amd_amd8111_config *conf;
	struct resource *resource;
	u8 *mmio;

	conf = dev->chip_info;
	resource = find_resource(dev, PCI_BASE_ADDRESS_0);
	mmio = res2mmio(resource, 0, 0);

	/* Hard Reset PHY */
	printk(BIOS_DEBUG, "Resetting PHY... ");
	if (conf->phy_lowreset) {
		write32((mmio + CMD3), VAL0 | PHY_RST_POL | RESET_PHY);
	} else {
		write32((mmio + CMD3), VAL0 | RESET_PHY);
	}
	mdelay(15);
	write32((mmio + CMD3), RESET_PHY);
	printk(BIOS_DEBUG, "Done\n");
}

static void lpci_set_subsystem(struct device *dev, unsigned vendor,
			       unsigned device)
{
	pci_write_config32(dev, 0xc8,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

static struct device_operations nic_ops  = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = nic_init,
	.scan_bus         = 0,
	.enable		  = amd8111_enable,
	.ops_pci	  = &lops_pci,
};

static const struct pci_driver nic_driver __pci_driver = {
	.ops    = &nic_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = PCI_DEVICE_ID_AMD_8111_NIC,
};
