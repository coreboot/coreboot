/*
 *
 * This file is part of the coreboot project.
 * Copyright (C) 2003 Linux Networx
 * Copyright (C) 2008 Ronald G. Minnich <rminnich@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
#include <types.h>
#include <lib.h>
#include <console.h>
#include <device/pci.h>
#include <msr.h>
#include <legacy.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
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
	struct southbridge_amd_amd8111_nic_config *conf;
	struct resource *resource;
	unsigned long mmio;

	conf = dev->device_configuration;
	resource = find_resource(dev, PCI_BASE_ADDRESS_0);
	mmio = resource->base;

	/* Hard Reset PHY */
	printk(BIOS_DEBUG, "Reseting PHY... ");
/*
	if (conf->phy_lowreset) {
		writel(VAL0 | PHY_RST_POL | RESET_PHY , (void *)(mmio + CMD3));
	} else {
		writel(VAL0 | RESET_PHY, (void *)(mmio + CMD3));
	}
	mdelay(15);
	writel(RESET_PHY, (void *)(mmio + CMD3));
*/
	printk(BIOS_DEBUG, "Done\n");
}

static void lpci_set_subsystem(struct device * dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0xc8,
		((device & 0xffff) << 16) | (vendor & 0xffff));
}

static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

struct device_operations amd8111_nic = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			 .device = PCI_DEVICE_ID_AMD_8111_NIC}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase3_chip_setup_dev	 = amd8111_enable,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = nic_init,
	.ops_pci		 = &lops_pci,
};

