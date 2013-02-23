/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 iWave Systems
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <arch/io.h>

static void sch_mmc_init(struct device *dev)
{
	u32 reg32;

	printk(BIOS_DEBUG, "MMC: Setting up controller.. ");
	reg32 = pci_read_config32(dev, PCI_COMMAND);
	reg32 |= PCI_COMMAND_MASTER;
	reg32 |= PCI_COMMAND_MEMORY;
	pci_write_config32(dev, PCI_COMMAND, reg32);
	printk(BIOS_DEBUG, "done.\n");
}

static void sch_mmc_set_subsystem(device_t dev, unsigned vendor,
				  unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations lops_pci = {
	.set_subsystem = &sch_mmc_set_subsystem,
};

static struct device_operations sch_mmc_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= sch_mmc_init,
	.scan_bus		= 0,
	.ops_pci		= &lops_pci,
};

static const struct pci_driver sch_mmc1 __pci_driver = {
	.ops	= &sch_mmc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x811C,
};

static const struct pci_driver sch_mmc2 __pci_driver = {
	.ops	= &sch_mmc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x811D,
};

static const struct pci_driver sch_mmc3 __pci_driver = {
	.ops	= &sch_mmc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x811E,

};
