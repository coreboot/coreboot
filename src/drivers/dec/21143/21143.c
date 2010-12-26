/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Marc Bertens <mbertens@xs4all.nl>
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

#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>

/**
 * The following should be set in the mainboard-specific Kconfig file.
 */
#if (!defined(CONFIG_DEC21143_CACHE_LINE_SIZE) || \
     !defined(CONFIG_DEC21143_EXPANSION_ROM_BASE_ADDRESS) || \
     !defined(CONFIG_DEC21143_COMMAND_AND_STATUS_CONFIGURATION))
#error "you must supply these values in your mainboard-specific Kconfig file"
#endif

/* CONFIG_DEC21143_CACHE_LINE_SIZE try 0x00000000 if unsure */
/* CONFIG_DEC21143_EXPANSION_ROM_BASE_ADDRESS try 0x00000000 if unsure */
/* CONFIG_DEC21143_COMMAND_AND_STATUS_CONFIGURATION try 0x02800107 or 0x02800007 if unsure */

/**
 * This driver takes the values from Kconfig and loads them in the registers.
 */
static void dec_21143_enable(device_t dev)
{
	printk(BIOS_DEBUG, "Initializing DECchip 21143\n");

	/* Command and status configuration (offset 0x04) */
	pci_write_config32(dev, 0x04,
			   CONFIG_DEC21143_COMMAND_AND_STATUS_CONFIGURATION);
	printk(BIOS_DEBUG, "0x04 = %08x (07 01 80 02)\n",
	       pci_read_config32(dev, 0x04));

	/* Cache line size (offset 0x0C) */
	pci_write_config8(dev, 0x0C, CONFIG_DEC21143_CACHE_LINE_SIZE);
	printk(BIOS_DEBUG, "0x0c = %08x (00 80 00 00)\n",
	       pci_read_config32(dev, 0x0C));

	/* Expansion ROM base address (offset 0x30) */
	pci_write_config32(dev, 0x30,
			   CONFIG_DEC21143_EXPANSION_ROM_BASE_ADDRESS);
	printk(BIOS_DEBUG, "0x30 = %08x (0x00000000)\n",
	       pci_read_config32(dev, 0x30));
}

static struct device_operations dec_21143_ops = {
	.read_resources   = pci_dev_read_resources,
	.set_resources    = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = dec_21143_enable,
	.scan_bus         = 0,
};

static const struct pci_driver dec_21143_driver __pci_driver = {
	.ops    = &dec_21143_ops,
	.vendor = PCI_VENDOR_ID_DEC,
	.device = PCI_DEVICE_ID_DEC_21142,
};
