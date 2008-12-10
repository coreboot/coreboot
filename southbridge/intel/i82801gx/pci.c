/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
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
#include <mainboard.h>

static void pci_init(struct device *dev)
{
	u16 reg16;

#if 0
	/* Commented out for now because it will break on some machines. */
	/* Set latency timer to 32. */
	pci_write_config16(dev, 0x1b, 0x20);
#endif

	/* disable parity error response */
	reg16 = pci_read_config16(dev, 0x3e);
	reg16 &= ~(1 << 0);
	pci_write_config16(dev, 0x3e, reg16);

	/* Clear errors in status registers */
	reg16 = pci_read_config16(dev, 0x06);
	reg16 |= 0xf900;
	pci_write_config16(dev, 0x06, reg16);

	reg16 = pci_read_config16(dev, 0x1e);
	reg16 |= 0xf900;
	pci_write_config16(dev, 0x1e, reg16);
}

static void ich_pci_dev_enable_resources(struct device *dev)
{
	const struct pci_operations *ops;

	/* Set the subsystem vendor and device id for mainboard devices */
	ops = ops_pci(dev);
	if (dev->on_mainboard && ops && ops->set_subsystem) {
		printk(BIOS_DEBUG, "%s subsystem <- %02x/%02x\n",
			dev_path(dev), 
			MAINBOARD_PCI_SUBSYSTEM_VENDOR_ID,
			MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID);
		ops->set_subsystem(dev, 
			MAINBOARD_PCI_SUBSYSTEM_VENDOR_ID,
			MAINBOARD_PCI_SUBSYSTEM_DEVICE_ID);
	}

#if 0
	u16 command;
	/* If we write to PCI_COMMAND, on some systems 
	 * this will cause the ROM and APICs not being visible
	 * anymore.
	 */
	command = pci_read_config16(dev, PCI_COMMAND);
	command |= dev->command;
	printk(BIOS_DEBUG, "%s cmd <- %02x\n", dev_path(dev), command);
	pci_write_config16(dev, PCI_COMMAND, command);
#endif
}

static void ich_pci_bus_enable_resources(struct device *dev)
{
	u16 ctrl;
	/* enable IO in command register if there is VGA card
	 * connected with (even it does not claim IO resource)
	 */
	if (dev->link[0].bridge_ctrl & PCI_BRIDGE_CTL_VGA)
		dev->command |= PCI_COMMAND_IO;
	ctrl = pci_read_config16(dev, PCI_BRIDGE_CONTROL);
	ctrl |= dev->link[0].bridge_ctrl;
	ctrl |= (PCI_BRIDGE_CTL_PARITY + PCI_BRIDGE_CTL_SERR); /* error check */
	printk(BIOS_DEBUG, "%s bridge ctrl <- %04x\n", dev_path(dev), ctrl);
	pci_write_config16(dev, PCI_BRIDGE_CONTROL, ctrl);

	/* This is the reason we need our own pci_bus_enable_resources */
	ich_pci_dev_enable_resources(dev);

	enable_childrens_resources(dev);
}

static void set_subsystem(struct device * dev, u16 vendor, u16 device)
{
#if 0
	/* Currently disabled because it causes a "BAR 9" memory resource
	 * conflict:
	 */
	u32 pci_id;

	printk(BIOS_DEBUG, "Setting PCI bridge subsystem ID\n");
	pci_id = pci_read_config32(dev, 0);
	pci_write_config32(dev, PCI_SUBSYSTEM_VENDOR_ID, pci_id );
#endif
}

static struct pci_operations pci_ops = {
	.set_subsystem = set_subsystem,
};


/* Desktop */
/* 82801BA/CA/DB/EB/ER/FB/FR/FW/FRW/GB/GR/GDH/HB/IB/6300ESB/i3100 */
struct device_operations i82801g_pci = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_INTEL,
			      .device = 0x244e}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_bus_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = ich_pci_bus_enable_resources,
	.phase6_init		 = pci_init,
	.ops_pci		 = &pci_dev_ops_pci,
};

/* Mobile / Ultra Mobile */
/* 82801BAM/CAM/DBL/DBM/FBM/GBM/GHM/GU/HBM/HEM */
struct device_operations i82801gmu_pci = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_INTEL,
			      .device = 0x2448}}},
	.constructor		 = default_device_constructor,
	.reset_bus		= pci_bus_reset,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_bus_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = ich_pci_bus_enable_resources,
	.phase6_init		 = pci_init,
	.ops_pci		 = &pci_dev_ops_pci,
};
