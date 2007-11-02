/*
 * This file is part of the LinuxBIOS project.
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
 *
 * Turn off machine check triggers when reading
 * pci space where there are no devices.
 * This is necessary when scaning the bus for
 * devices which is done by the kernel
 *
 * written in 2003 by Eric Biederman
 *
 *  - Athlon64 workarounds by Stefan Reinauer
 *  - "reset once" logic by Yinghai Lu
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
 *
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <part/hard_reset.h>
#include <pc80/mc146818rtc.h>
#include <bitops.h>
#include <cpu/amd/model_fxx_rev.h>

//#include "amdk8.h"

#include <arch/io.h>

/**
 * @brief Read resources for AGP aperture
 *
 * @param
 *
 * There is only one AGP aperture resource needed. The resoruce is added to
 * the northbridge of BSP.
 *
 * The same trick can be used to augment legacy VGA resources which can
 * be detect by generic pci reousrce allocator for VGA devices.
 * BAD: it is more tricky than I think, the resource allocation code is
 * implemented in a way to NOT DOING legacy VGA resource allcation on
 * purpose :-(.
 */


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



static void sis761_read_resources(device_t dev)
{
	struct resource *resource;
	unsigned char iommu;
	/* Read the generic PCI resources */
	printk_debug("sis761_read_resources\n");
	pci_dev_read_resources(dev);

	/* If we are not the first processor don't allocate the gart apeture */
	if (dev->path.u.pci.devfn != PCI_DEVFN(0x0, 0)) {
		return;
	}


	return;

	iommu = 1;
	get_option(&iommu, "iommu");

	if (iommu) {
		/* Add a Gart apeture resource */
		resource = new_resource(dev, 0x94);
		resource->size = iommu?AGP_APERTURE_SIZE:1;
		resource->align = log2(resource->size);
		resource->gran  = log2(resource->size);
		resource->limit = 0xffffffff; /* 4G */
		resource->flags = IORESOURCE_MEM;
	}
}

static void set_agp_aperture(device_t dev)
{
	struct resource *resource;

	return;

	resource = probe_resource(dev, 0x94);
	if (resource) {
		device_t pdev;
		uint32_t gart_base, gart_acr;

		/* Remember this resource has been stored */
		resource->flags |= IORESOURCE_STORED;

		/* Find the size of the GART aperture */
		gart_acr = (0<<6)|(0<<5)|(0<<4)|((resource->gran - 25) << 1)|(0<<0);

		/* Get the base address */
		gart_base = ((resource->base) >> 25) & 0x00007fff;

		/* Update the other northbriges */
		pdev = 0;
		while((pdev = dev_find_device(PCI_VENDOR_ID_AMD, 0x1103, pdev))) {
			/* Store the GART size but don't enable it */
			pci_write_config32(pdev, 0x90, gart_acr);

			/* Store the GART base address */
			pci_write_config32(pdev, 0x94, gart_base);

			/* Don't set the GART Table base address */
			pci_write_config32(pdev, 0x98, 0);

			/* Report the resource has been stored... */
			report_resource_stored(pdev, resource, " <gart>");
		}
	}
}

static void sis761_set_resources(device_t dev)
{
        printk_debug("sis761_set_resources ------->\n");
	/* Set the gart apeture */
//	set_agp_aperture(dev);

	/* Set the generic PCI resources */
	pci_dev_set_resources(dev);
	printk_debug("sis761_set_resources <-------\n");
}

static void sis761_init(struct device *dev)
{
	uint32_t cmd, cmd_ref;
	int needs_reset;
	struct device *f0_dev, *f2_dev;
	msr_t	msr;


	needs_reset = 0;
	printk_debug("sis761_init: ---------->\n");

	msr = rdmsr(0xC001001A);
	pci_write_config16(dev, 0x8E, msr.lo >> 16);				// Topbound
	pci_write_config8(dev, 0x7F, 0x08);			// ACPI Base
	outb(inb(0x856) | 0x40, 0x856);	 // Auto-Reset Function

	printk_debug("sis761_init: <----------\n");
	printk_debug("done.\n");
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
