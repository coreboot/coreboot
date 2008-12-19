/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Yinghai Lu, Tyan
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

static void agp3bridge_init(struct device * dev)
{
	u8 byte;

	/* Enable BM, MEM and IO */
	/* this config32 is arguably wrong but I won't change until we can test. */
	byte = pci_read_config32(dev, PCI_COMMAND);
	byte |= 0x07;
	pci_write_config8(dev, PCI_COMMAND, byte);

	return;
}

struct device_operations amd8151_agp3bridge = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			 .device = PCI_DEVICE_ID_AMD_8151_AGP}}},
	.constructor		 = default_device_constructor,
	.phase3_scan		 = pci_scan_bridge,
	.phase4_read_resources	 = pci_bus_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_bus_enable_resources,
	.phase6_init		 = agp3bridge_init,
};

static void agp3dev_enable(struct device * dev)
{
	u32 value;
	
	/* AGP enable */
	value = pci_read_config32(dev, 0xa8);
	value |= (3<<8)|2; //AGP 8x
	pci_write_config32(dev, 0xa8, value);

	/* enable BM and MEM */
	value = pci_read_config32(dev, PCI_COMMAND);
	value |= 6;
	pci_write_config32(dev, PCI_COMMAND, value);
#if 0
	/* FIXME: should we add agp aperture base and size here ?
	 * or it is done by AGP drivers */
#endif
}

static struct pci_operations pci_ops_pci_dev = {
        .set_subsystem    = pci_dev_set_subsystem,
};

struct device_operations amd8151_agp3dev = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_AMD,
			 .device = PCI_DEVICE_ID_AMD_8151_SYSCTRL}}},
	.constructor		 = default_device_constructor,
	.phase3_enable		 = agp3dev_enable,
	.phase4_read_resources	 = pci_dev_read_resources,
	.phase4_set_resources	 = pci_set_resources,
	.phase5_enable_resources = pci_dev_enable_resources,
	.phase6_init		 = NULL,
	.ops_pci		 = &pci_dev_ops_pci,
};
