/*
 * This file is part of the coreboot project.
 *
 * written in 2003 by Eric Biederman
 *
 *  - Athlon64 workarounds by Stefan Reinauer
 *  - "reset once" logic by Yinghai Lu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/* Turn off machine check triggers when reading
 * PCI space where there are no devices.
 * This is necessary when scanning the bus for
 * devices which is done by the kernel
 *
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <reset.h>
#include <pc80/mc146818rtc.h>
#include <lib.h>
#include <cpu/amd/model_fxx_rev.h>

#include "amdk8.h"

/**
 * @brief Read resources for AGP aperture
 *
 * @param dev
 *
 * There is only one AGP aperture resource needed. The resource is added to
 * the northbridge of BSP.
 *
 * The same trick can be used to augment legacy VGA resources which can
 * be detect by generic PCI resource allocator for VGA devices.
 * BAD: it is more tricky than I think, the resource allocation code is
 * implemented in a way to NOT DOING legacy VGA resource allocation on
 * purpose :-(.
 */
static void mcf3_read_resources(struct device *dev)
{
	struct resource *resource;
	unsigned char iommu;
	/* Read the generic PCI resources */
	pci_dev_read_resources(dev);

	/* If we are not the first processor don't allocate the GART aperture */
	if (dev->path.pci.devfn != PCI_DEVFN(0x18, 3)) {
		return;
	}

	iommu = CONFIG_IOMMU;
	get_option(&iommu, "iommu");

	if (iommu) {
		/* Add a GART aperture resource */
		resource = new_resource(dev, 0x94);
		resource->size = CONFIG_AGP_APERTURE_SIZE;
		resource->align = log2(resource->size);
		resource->gran  = log2(resource->size);
		resource->limit = 0xffffffff; /* 4G */
		resource->flags = IORESOURCE_MEM;
	}
}

static void set_agp_aperture(struct device *dev)
{
	struct resource *resource;

	resource = probe_resource(dev, 0x94);
	if (resource) {
		struct device *pdev;
		uint32_t gart_base, gart_acr;

		/* Remember this resource has been stored */
		resource->flags |= IORESOURCE_STORED;

		/* Find the size of the GART aperture */
		gart_acr = (0<<6)|(0<<5)|(0<<4)|((resource->gran - 25) << 1)|(0<<0);

		/* Get the base address */
		gart_base = ((resource->base) >> 25) & 0x00007fff;

		/* Update the other northbridges */
		pdev = 0;
		while ((pdev = dev_find_device(PCI_VENDOR_ID_AMD, 0x1103, pdev))) {
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

static void mcf3_set_resources(struct device *dev)
{
	/* Set the gart apeture */
	set_agp_aperture(dev);

	/* Set the generic PCI resources */
	pci_dev_set_resources(dev);
}

static void misc_control_init(struct device *dev)
{
	uint32_t cmd, cmd_ref;
	int needs_reset;
	struct device *f0_dev;

	printk(BIOS_DEBUG, "NB: Function 3 Misc Control.. ");
	needs_reset = 0;

	/* Disable Machine checks from Invalid Locations.
	 * This is needed for PC backwards compatibility.
	 */
	cmd = pci_read_config32(dev, 0x44);
	cmd |= (1<<6) | (1<<25);
	pci_write_config32(dev, 0x44, cmd);
#if !IS_ENABLED(CONFIG_K8_REV_F_SUPPORT)
	if (is_cpu_pre_c0()) {

		/* Errata 58
		 * Disable CPU low power states C2, C1 and throttling
		 */
		cmd = pci_read_config32(dev, 0x80);
		cmd &= ~(1<<0);
		pci_write_config32(dev, 0x80, cmd);
		cmd = pci_read_config32(dev, 0x84);
		cmd &= ~(1<<24);
		cmd &= ~(1<<8);
		pci_write_config32(dev, 0x84, cmd);

		/* Errata 66
		 * Limit the number of downstream posted requests to 1
		 */
		cmd = pci_read_config32(dev, 0x70);
		if ((cmd & (3 << 0)) != 2) {
			cmd &= ~(3<<0);
			cmd |= (2<<0);
			pci_write_config32(dev, 0x70, cmd);
			needs_reset = 1;
		}
		cmd = pci_read_config32(dev, 0x7c);
		if ((cmd & (3 << 4)) != 0) {
			cmd &= ~(3<<4);
			cmd |= (0<<4);
			pci_write_config32(dev, 0x7c, cmd);
			needs_reset = 1;
		}
		/* Clock Power/Timing Low */
		cmd = pci_read_config32(dev, 0xd4);
		if (cmd != 0x000D0001) {
			cmd = 0x000D0001;
			pci_write_config32(dev, 0xd4, cmd);
			needs_reset = 1; /* Needed? */
		}
	}
	else if (is_cpu_pre_d0()) {
		struct device *f2_dev;
		uint32_t dcl;
		f2_dev = dev_find_slot(0, dev->path.pci.devfn - 3 + 2);
		/* Errata 98
		 * Set Clk Ramp Hystersis to 7
		 * Clock Power/Timing Low
		 */
		cmd_ref = 0x04e20707; /* Registered */
		dcl = pci_read_config32(f2_dev, DRAM_CONFIG_LOW);
		if (dcl & DCL_UnBuffDimm) {
			cmd_ref = 0x000D0701; /* Unbuffered */
		}
		cmd = pci_read_config32(dev, 0xd4);
		if (cmd != cmd_ref) {
			pci_write_config32(dev, 0xd4, cmd_ref);
			needs_reset = 1; /* Needed? */
		}
	}
#endif
	/* Optimize the Link read pointers */
	f0_dev = dev_find_slot(0, dev->path.pci.devfn - 3);
	if (f0_dev) {
		int link;
		cmd_ref = cmd = pci_read_config32(dev, 0xdc);
		for (link = 0; link < 3; link++) {
			uint32_t link_type;
			unsigned reg;
			/* This works on an Athlon64 because unimplemented links return 0 */
			reg = 0x98 + (link * 0x20);
			link_type = pci_read_config32(f0_dev, reg);
			/* Only handle coherent link here please */
			if ((link_type & (LinkConnected|InitComplete|NonCoherent))
				== (LinkConnected|InitComplete))
			{
				cmd &= ~(0xff << (link *8));
				/* FIXME this assumes the device on the other side is an AMD device */
				cmd |= 0x25 << (link *8);
			}
		}
		if (cmd != cmd_ref) {
			pci_write_config32(dev, 0xdc, cmd);
			needs_reset = 1;
		}
	}
	else {
		printk(BIOS_ERR, "Missing f0 device!\n");
	}
	if (needs_reset) {
		printk(BIOS_DEBUG, "resetting cpu\n");
		hard_reset();
	}
	printk(BIOS_DEBUG, "done.\n");
}


static struct device_operations mcf3_ops  = {
	.read_resources   = mcf3_read_resources,
	.set_resources    = mcf3_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init             = misc_control_init,
	.scan_bus         = 0,
	.ops_pci          = 0,
};

static const struct pci_driver mcf3_driver __pci_driver = {
	.ops    = &mcf3_ops,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1103,
};
