/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Linux NetworX
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
#include "amd8111.h"

static void ide_init(struct device *dev)
{
	struct southbridge_amd_amd8111_ide_config *conf;
	/* Enable ide devices so the linux ide driver will work */
	u16 word;
	u8 byte;
	conf = dev->device_configuration;

	word = pci_read_config16(dev, 0x40);
	/* Ensure prefetch is disabled */
	word &= ~((1 << 15) | (1 << 13));
	if (conf->ide1_enable) {
		/* Enable secondary ide interface */
		word |= (1<<0);
		printk(BIOS_DEBUG, "IDE1 ");
	}
	if (conf->ide0_enable) {
		/* Enable primary ide interface */
		word |= (1<<1);
		printk(BIOS_DEBUG, "IDE0 ");
	}

	word |= (1<<12);
	word |= (1<<14);

	pci_write_config16(dev, 0x40, word);


	byte = 0x20 ; // Latency: 64-->32
	pci_write_config8(dev, PCI_LATENCY_TIMER, byte);

	word = 0x0f;
	pci_write_config16(dev, 0x42, word);
}

static void lpci_set_subsystem(struct device * dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x70, 
		((device & 0xffff) << 16) | (vendor & 0xffff));
}
static struct pci_operations lops_pci = {
	.set_subsystem = lpci_set_subsystem,
};

struct device_operations amd8111_ide = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			      .device = PCI_DEVICE_ID_AMD_8111_IDE}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = 0,
	.phase3_chip_setup_dev           = amd8111_enable,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_dev_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = ide_init,
	.ops_pci          = &lops_pci
};
