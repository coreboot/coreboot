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

/* PCI Configuration Space (D31:F1): IDE */
#define INTR_LN			0x3c
#define IDE_TIM_PRI		0x80	/* IDE timings, primary */

extern int sch_port_access_read(int port, int reg, int bytes);

static void ide_init(struct device *dev)
{
	u32 ideTimingConfig, reg32;

	printk(BIOS_DEBUG, "sch_ide: initializing... ");

	reg32 = pci_read_config32(dev, PCI_COMMAND);
	pci_write_config32(dev, PCI_COMMAND,
			   reg32 | PCI_COMMAND_IO | PCI_COMMAND_MASTER);

	/* Program the clock. */
	if (sch_port_access_read(5, 3, 4) & (1 << 3)) {
		/* 533MHz, Read PCI MC register */
		reg32 = pci_read_config32(dev, 0x60);
		pci_write_config32(dev, 0x60, reg32 | 1);
	} else {
		/* 400MHz */
		reg32 = pci_read_config32(dev, 0x60);
		reg32 &= ~1;
		pci_write_config32(dev, 0x60, reg32);
	}

	/* Enable primary IDE interface. 80=04 81=00 82=02 83=80 */
	ideTimingConfig = 0x80020000;
	printk(BIOS_DEBUG, "IDE0 ");
	pci_write_config32(dev, IDE_TIM_PRI, ideTimingConfig);

	/* Set Interrupt Line. */
	/* Interrupt Pin is set by D31IP.PIP */
	printk(BIOS_DEBUG, "\n");
}

static void ide_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	if (!vendor || !device) {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				   pci_read_config32(dev, PCI_VENDOR_ID));
	} else {
		pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID,
				((device & 0xffff) << 16) | (vendor & 0xffff));
	}
}

static struct pci_operations ide_pci_ops = {
	.set_subsystem = ide_set_subsystem,
};

static struct device_operations ide_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init,
	.scan_bus		= 0,
	.ops_pci		= &ide_pci_ops,
};

static const struct pci_driver sch_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x811A,
};
