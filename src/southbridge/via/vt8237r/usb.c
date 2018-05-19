/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 Jon Harrison <bothlyn@blueyonder.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
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
#include "chip.h"
#include "vt8237r.h"

#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
u32 usb_io_addr[4] = {0xcc00, 0xd000, 0xd400, 0xd800};
#endif

static void usb_i_init(struct device *dev)
{
#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	u8 reg8;

	printk(BIOS_DEBUG, "Entering %s\n", __func__);

	reg8 = pci_read_config8(dev, 0x04);

	printk(BIOS_SPEW, "%s Read %02X from PCI Command Reg\n", dev_path(dev), reg8);

	reg8 = reg8 | PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY;
	pci_write_config8(dev, 0x04, reg8);

	printk(BIOS_SPEW, "%s Wrote %02X to PCI Command Reg\n", dev_path(dev), reg8);

	/* Set Cache Line Size and Latency Timer */
	pci_write_config8(dev, 0x0c, 0x08);
	pci_write_config8(dev, 0x0d, 0x20);

	/* Enable Sub Device ID Back Door and set Generic */
	reg8 = pci_read_config8(dev, 0x42);
	reg8 |= 0x10;
	pci_write_config8(dev, 0x42, reg8);
	pci_write_config16(dev, 0x2e, 0xAA07);
	reg8 &= ~0x10;
	pci_write_config8(dev, 0x42, reg8);


	pci_write_config8(dev, 0x41, 0x12);

	pci_write_config8(dev, 0x49, 0x0B);

	/* Clear PCI Status */
	pci_write_config16(dev, 0x06, 0x7A10);
#endif
	return;
}

static void vt8237_usb_i_read_resources(struct device *dev)
{
#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	struct resource *res;
	u8 function = (u8) dev->path.pci.devfn & 0x7;

	printk(BIOS_SPEW, "VT8237R Fixing USB 1.1 fn %d I/O resource = 0x%04X\n", function, usb_io_addr[function]);

	/* Fix the I/O Resources of the USB1.1 Interfaces */
	/* Auto PCI probe seems to size the resources     */
	/* Incorrectly                                    */
	res = new_resource(dev, PCI_BASE_ADDRESS_4);
	res->base = usb_io_addr[function];
	res->size = 256;
	res->limit = 0xffffUL;
	res->align = 10;
	res->gran = 8;
	res->flags = IORESOURCE_IO | IORESOURCE_FIXED |
		     		IORESOURCE_ASSIGNED;
#else
	pci_dev_read_resources(dev);
#endif
	return;
}

static void usb_ii_init(struct device *dev)
{
	struct southbridge_via_vt8237r_config *cfg;
#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	u8 reg8;

	printk(BIOS_DEBUG, "Entering %s\n", __func__);

	/* Set memory Write and Invalidate */
	reg8 = pci_read_config8(dev, 0x04);
	reg8 |= 0x10;
	pci_write_config8(dev, 0x04, reg8);

	/* Set Cache line Size and Latency Timer */
	pci_write_config8(dev, 0x0c, 0x08);
	pci_write_config8(dev, 0x0d, 0x20);

	/* Clear PCI Status */
	pci_write_config16(dev, 0x06, 0x7A10);
#endif

	cfg = dev->chip_info;

	if (cfg) {
		if (cfg->usb2_termination_set) {
			/* High Speed Port Pad Termination Resistor Fine Tune */
			pci_write_config8(dev, 0x5a, cfg->usb2_termination_c |
				(cfg->usb2_termination_d << 4));
			pci_write_config8(dev, 0x5b, cfg->usb2_termination_a |
				(cfg->usb2_termination_b << 4));
			pci_write_config8(dev, 0x5d, cfg->usb2_termination_e |
				(cfg->usb2_termination_f << 4));
			pci_write_config8(dev, 0x5e, cfg->usb2_termination_g |
				(cfg->usb2_termination_h << 4));
		}

		if (cfg->usb2_dpll_set) {
			/* Delay DPLL Input Data Control */
			pci_write_config8(dev, 0x5c,
				(pci_read_config8(dev, 0x5c) & ~0x70) |
				(cfg->usb2_dpll_delay << 4));
		}
	}
}

static void vt8237_usb_ii_read_resources(struct device *dev)
{
#if IS_ENABLED(CONFIG_EPIA_VT8237R_INIT)
	struct resource *res;

	/* Fix the I/O Resources of the USB2.0 Interface */
	res = new_resource(dev, PCI_BASE_ADDRESS_0);
	res->base = 0xF6000000ULL;
	res->size = 256;
	res->align = 12;
	res->gran = 8;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
		     		IORESOURCE_ASSIGNED;
#else
	pci_dev_read_resources(dev);
#endif
	return;
}

static void vt8237_set_subsystem(struct device *dev, unsigned vendor,
				 unsigned device)
{
	pci_write_config32(dev, 0x42, pci_read_config32(dev, 0x42) | 0x10);
	pci_write_config16(dev, 0x2c, vendor);
	pci_write_config16(dev, 0x2e, device);
	pci_write_config32(dev, 0x42, pci_read_config32(dev, 0x42) & ~0x10);
}

static struct pci_operations lops_pci = {
	.set_subsystem = vt8237_set_subsystem,
};

static const struct device_operations usb_i_ops = {
	.read_resources		= vt8237_usb_i_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init				= usb_i_init,
	.enable				= 0,
	.ops_pci			= &lops_pci,
};

static const struct device_operations usb_ii_ops = {
	.read_resources		= vt8237_usb_ii_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init				= usb_ii_init,
	.enable				= 0,
	.ops_pci			= &lops_pci,
};

static const struct pci_driver vt8237r_driver_usbii __pci_driver = {
	.ops	= &usb_ii_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237R_EHCI,
};

static const struct pci_driver vt8237r_driver_usbi __pci_driver = {
	.ops	= &usb_i_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_VT8237R_UHCI,
};
