/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
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
 */

/*
 * Turn off machine check triggers when reading
 * pci space where there are no devices.
 * This is necessary when scanning the bus for
 * devices which is done by the kernel
 *
 * written in 2003 by Eric Biederman
 *
 *  - Athlon64 workarounds by Stefan Reinauer
 *  - "reset once" logic by Yinghai Lu
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <cpu/amd/model_fxx_rev.h>
#include <arch/io.h>

typedef struct msr_struct
{
	unsigned lo;
	unsigned hi;
} msr_t;

static inline msr_t rdmsr(unsigned index)
{
	msr_t result;
	result.lo = 0;
	result.hi = 0;
	return result;
}

/**
 * Read resources for AGP aperture.
 *
 * There is only one AGP aperture resource needed. The resource is added to
 * the northbridge of BSP.
 *
 * The same trick can be used to augment legacy VGA resources which can
 * be detect by generic PCI resource allocator for VGA devices.
 * BAD: it is more tricky than I think, the resource allocation code is
 * implemented in a way to NOT DOING legacy VGA resource allocation on
 * purpose :-(.
 *
 * @param dev TODO
 */
static void sis761_read_resources(struct device *dev)
{
	/* Read the generic PCI resources */
	printk(BIOS_DEBUG, "sis761_read_resources ------->\n");
	pci_dev_read_resources(dev);

	/* If we are not the first processor don't allocate the gart aperture */
	if (dev->path.pci.devfn != PCI_DEVFN(0x0, 0)) {
		printk(BIOS_DEBUG, "sis761_not_the_first_processor !!!\n");
		return;
	}

	printk(BIOS_DEBUG, "sis761_read_resources <-------\n");
	return;

}

static void sis761_set_resources(struct device *dev)
{
	printk(BIOS_DEBUG, "sis761_set_resources ------->\n");

	/* Set the generic PCI resources */
	pci_dev_set_resources(dev);
	printk(BIOS_DEBUG, "sis761_set_resources <-------\n");
}

static void sis761_init(struct device *dev)
{
	int needs_reset;
	msr_t	msr;


	needs_reset = 0;
	printk(BIOS_DEBUG, "sis761_init: ---------->\n");

	msr = rdmsr(0xC001001A);
	pci_write_config16(dev, 0x8E, msr.lo >> 16);				// Topbound
	pci_write_config8(dev, 0x7F, 0x08);			// ACPI Base
	outb(inb(0x856) | 0x40, 0x856);	 // Auto-Reset Function

	printk(BIOS_DEBUG, "sis761_init: <----------\n");
}


static struct device_operations sis761_ops  = {
	.read_resources   = sis761_read_resources,
	.set_resources    = sis761_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = sis761_init,
	.scan_bus         = 0,
	.ops_pci          = 0,
};

static const struct pci_driver sis761_driver __pci_driver = {
	.ops    = &sis761_ops,
	.vendor = PCI_VENDOR_ID_SIS,
	.device = PCI_DEVICE_ID_SIS_SIS761,
};
