/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 Tyan Computer
 * (Written by Yinghai Lu <yinghailu@gmail.com> for Tyan Computer)
 * Copyright (C) 2005 Digital Design Corporation
 * (Written by Steven J. Magnani <steve@digidescorp.com> for Digital Design)
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801ax.h"

typedef struct southbridge_intel_i82801ax_config config_t;

static void ide_init(struct device *dev)
{
	u16 reg16;
	config_t *conf = dev->chip_info;

	reg16 = pci_read_config16(dev, IDE_TIM_PRI);
	reg16 &= ~IDE_DECODE_ENABLE;
	if (!conf || conf->ide0_enable)
		reg16 |= IDE_DECODE_ENABLE;
	printk(BIOS_DEBUG, "IDE: %s IDE interface: %s\n", "Primary",
	       conf->ide0_enable ? "on" : "off");
	pci_write_config16(dev, IDE_TIM_PRI, reg16);

	reg16 = pci_read_config16(dev, IDE_TIM_SEC);
	reg16 &= ~IDE_DECODE_ENABLE;
	if (!conf || conf->ide1_enable)
		reg16 |= IDE_DECODE_ENABLE;
	printk(BIOS_DEBUG, "IDE: %s IDE interface: %s\n", "Secondary",
	       conf->ide0_enable ? "on" : "off");
	pci_write_config16(dev, IDE_TIM_SEC, reg16);
}

static struct device_operations ide_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init,
	.scan_bus		= 0,
	.enable			= i82801ax_enable,
};

/* 82801AA (ICH) */
static const struct pci_driver i82801aa_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2411,
};

/* 82801AB (ICH0) */
static const struct pci_driver i82801ab_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2421,
};
