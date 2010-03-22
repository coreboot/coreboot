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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "i82801ax.h"

typedef struct southbridge_intel_i82801ax_config config_t;

static void ide_init(struct device *dev)
{
	/* Get the chip configuration */
	config_t *config = dev->chip_info; 

	/* TODO: Needs to be tested for compatibility with ICH5(R). */
	/* Enable IDE devices so the Linux IDE driver will work. */
	uint16_t ideTimingConfig;

	ideTimingConfig = pci_read_config16(dev, IDE_TIM_PRI);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	if (!config || config->ide0_enable) {
		/* Enable primary IDE interface. */
		ideTimingConfig |= IDE_DECODE_ENABLE;
		printk(BIOS_DEBUG, "IDE0: Primary IDE interface is enabled\n");
	} else {
		printk(BIOS_INFO, "IDE0: Primary IDE interface is disabled\n");
	}
	pci_write_config16(dev, IDE_TIM_PRI, ideTimingConfig);

	ideTimingConfig = pci_read_config16(dev, IDE_TIM_SEC);
	ideTimingConfig &= ~IDE_DECODE_ENABLE;
	if (!config || config->ide1_enable) {
		/* Enable secondary IDE interface. */
		ideTimingConfig |= IDE_DECODE_ENABLE;
		printk(BIOS_DEBUG, "IDE1: Secondary IDE interface is enabled\n");
	} else {
		printk(BIOS_INFO, "IDE1: Secondary IDE interface is disabled\n");
	}
	pci_write_config16(dev, IDE_TIM_SEC, ideTimingConfig);
}

static struct device_operations ide_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init			= ide_init,
	.scan_bus		= 0,
	.enable			= i82801ax_enable,
};

/* 82801AA */
static const struct pci_driver i82801aa_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2411,
};

/* 82801AB */
static const struct pci_driver i82801ab_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x2421,
};

/* 82801BA */
static const struct pci_driver i82801ba_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x244b,
};

/* 82801CA */
static const struct pci_driver i82801ca_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x248b,
};

/* 82801DB */
static const struct pci_driver i82801db_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x24cb,
};

/* 82801DBM */
static const struct pci_driver i82801dbm_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x24ca,
};

/* 82801EB & 82801ER */
static const struct pci_driver i82801ex_ide __pci_driver = {
	.ops	= &ide_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= 0x24db,
};
