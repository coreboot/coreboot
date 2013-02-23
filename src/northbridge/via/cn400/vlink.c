/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Jon Harrison <bothlyn@blueyonder.co.uk>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <console/console.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "northbridge.h"
#include "cn400.h"

static void noop_1k(u32 knops)
{
	u32 i;

	for (i = 0; i < 1024 * knops; i++) {
		__asm__ volatile ("nop\n\t");
	}

	return;
}

/* Vlink Performance Improvements */
static void vlink_init(device_t dev)
{
	u8 reg, reg8;
	int i, j;

	printk(BIOS_SPEW, "Entering CN400 %s\n", __func__);

	/* Disconnect the VLink Before Changing Settings */
	reg = pci_read_config8(dev, 0x47);
	reg |= 0x04;
	pci_write_config8(dev, 0x47, reg);

	/* Wait for anything pending to flush */
	noop_1k(20);

	/* Setup Vlink Mode 1 */
	pci_write_config8(dev, 0x4F, 0x01);
	pci_write_config8(dev, 0x48, 0x13);

	/* PCI Buffer Control */
	pci_write_config8(dev, 0x70, 0x82);

	/* CPU to PCI Flow Control */
	pci_write_config8(dev, 0x71, 0xc8);
	pci_write_config8(dev, 0x72, 0xee);

	/* PCI Master Control */
	pci_write_config8(dev, 0x73, 0x01);
	pci_write_config8(dev, 0x74, 0x20);

	/* PCI Arbitration 1 */
	pci_write_config8(dev, 0x75, 0x0f);

	/* PCI Arbitration 2 */
	pci_write_config8(dev, 0x76, 0x50);
	pci_write_config8(dev, 0x77, 0x6e);
	pci_write_config8(dev, 0x7F, 0x10);

	pci_write_config8(dev, 0x94, 0x20);
	pci_write_config8(dev, 0x95, 0x0f);

	/* V-Link CKG Control 1 */
	pci_write_config8(dev, 0xB0, 0x01);

	/* V-Link NB Compensation Control */
	pci_write_config8(dev, 0xB5, 0x46);
	pci_write_config8(dev, 0xB6, 0x68);
	reg = pci_read_config8(dev, 0xB4);
	reg |= 0x01;
	pci_write_config8(dev, 0xB4, reg);

	/* V-Link NB Receive Strobe Delay */
	pci_write_config8(dev, 0xB7, 0x02);

	/* V-Link SB Compensation Control */
	pci_write_config8(dev, 0xB9, 0x84);
	reg = pci_read_config8(dev, 0xB8);
	reg |= 0x01;
	pci_write_config8(dev, 0xB8, reg);

	pci_write_config8(dev, 0xBA, 0x6a);
	pci_write_config8(dev, 0xBB, 0x01);

#ifdef DEBUG_CN400
	/* Reconnect the VLink Before Continuing*/
	reg = pci_read_config8(dev, 0x47);
	reg &= ~0x04;
	pci_write_config8(dev, 0x47, reg);

	printk(BIOS_SPEW, "%s PCI Header Regs::\n", dev_path(dev));

	for (i = 0 ; i < 16; i++)
	{
		printk(BIOS_SPEW, "%02X: ", i*16);
		for (j = 0; j < 16; j++)
		{
			reg8 = pci_read_config8(dev, j+(i*16));
			printk(BIOS_SPEW, "%02X ", reg8);
		}
		printk(BIOS_SPEW, "\n");
	}
#endif
}

static const struct device_operations vlink_operations = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = vlink_init,
	.ops_pci          = 0,
};

static const struct pci_driver vlink_driver __pci_driver = {
	.ops    = &vlink_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN400_VLINK,
};

static void c3_host_init(device_t dev)
{
	u8 reg8;
	int i, j;

	printk(BIOS_SPEW, "Entering CN400 %s\n", __func__);

	printk(BIOS_SPEW, "%s PCI Header Regs::\n", dev_path(dev));

	for (i = 0 ; i < 16; i++)
	{
		printk(BIOS_SPEW, "%02X: ", i*16);
		for (j = 0; j < 16; j++)
		{
			reg8 = pci_read_config8(dev, j+(i*16));
			printk(BIOS_SPEW, "%02X ", reg8);
		}
		printk(BIOS_SPEW, "\n");
	}

}

static const struct device_operations c3_host_operations = {
	.read_resources   = cn400_noop,
	.set_resources    = cn400_noop,
	.enable_resources = cn400_noop,
	.init             = c3_host_init,
	.ops_pci          = 0,
};

static const struct pci_driver c3_host_driver __pci_driver = {
	.ops    = &c3_host_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN400_HOST,
};


static void c3_err_init(device_t dev)
{
	u8 reg8;
	int i, j;

	printk(BIOS_SPEW, "Entering CN400 %s\n", __func__);

	printk(BIOS_SPEW, "%s PCI Header Regs::\n", dev_path(dev));

	for (i = 0 ; i < 16; i++)
	{
		printk(BIOS_SPEW, "%02X: ", i*16);
		for (j = 0; j < 16; j++)
		{
			reg8 = pci_read_config8(dev, j+(i*16));
			printk(BIOS_SPEW, "%02X ", reg8);
		}
		printk(BIOS_SPEW, "\n");
	}

}

static const struct device_operations c3_err_operations = {
	.read_resources   = cn400_noop,
	.set_resources    = cn400_noop,
	.enable_resources = cn400_noop,
	.init             = c3_err_init,
	.ops_pci          = 0,
};

static const struct pci_driver c3_err_driver __pci_driver = {
	.ops    = &c3_err_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN400_ERR,
};

static void cn400_pm_init(device_t dev)
{
	u8 reg8;
	int i, j;

	printk(BIOS_SPEW, "Entering CN400 %s\n", __func__);

	printk(BIOS_SPEW, "%s PCI Header Regs::\n", dev_path(dev));

	for (i = 0 ; i < 16; i++)
	{
		printk(BIOS_SPEW, "%02X: ", i*16);
		for (j = 0; j < 16; j++)
		{
			reg8 = pci_read_config8(dev, j+(i*16));
			printk(BIOS_SPEW, "%02X ", reg8);
		}
		printk(BIOS_SPEW, "\n");
	}

}

static const struct device_operations cn400_pm_operations = {
	.read_resources   = cn400_noop,
	.set_resources    = cn400_noop,
	.enable_resources = cn400_noop,
	.init             = cn400_pm_init,
	.ops_pci          = 0,
};

static const struct pci_driver cn400_pm_driver __pci_driver = {
	.ops    = &c3_err_operations,
	.vendor = PCI_VENDOR_ID_VIA,
	.device = PCI_DEVICE_ID_VIA_CN400_PM,
};
