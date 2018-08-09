/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 by Eric Biederman
 * Copyright (C) Stefan Reinauer
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
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
 * pci space where there are no devices.
 * This is necessary when scanning the bus for
 * devices which is done by the kernel
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <pc80/mc146818rtc.h>
#include <lib.h>
#include <cbmem.h>
#include <cpu/amd/model_10xxx_rev.h>

#include "amdfam10.h"

/**
 * @brief Read resources for AGP aperture
 *
 * @param dev
 *
 * There is only one AGP aperture resource needed. The resource is added to
 * the northbridge of BSP.
 *
 * The same trick can be used to augment legacy VGA resources which can
 * be detect by generic pci reousrce allocator for VGA devices.
 * BAD: it is more tricky than I think, the resource allocation code is
 * implemented in a way to NOT DOING legacy VGA resource allocation on
 * purpose :-(.
 */
static void mcf3_read_resources(struct device *dev)
{
	struct resource *resource;
	unsigned char gart;
	/* Read the generic PCI resources */
	pci_dev_read_resources(dev);

	/* If we are not the first processor don't allocate the gart apeture */
	if (dev->path.pci.devfn != PCI_DEVFN(CONFIG_CDB, 3)) {
		return;
	}

	gart = 1;
	get_option(&gart, "gart");

	if (gart) {
		/* Add a Gart apeture resource */
		resource = new_resource(dev, 0x94);
		resource->size = CONFIG_AGP_APERTURE_SIZE;
		resource->align = log2(resource->size);
		resource->gran  = log2(resource->size);
		resource->limit = 0xffffffff; /* 4G */
		resource->flags = IORESOURCE_MEM;
	}
}

static void set_agp_aperture(struct device *dev, uint32_t pci_id)
{
	uint32_t dword;
	struct resource *resource;

	resource = probe_resource(dev, 0x94);
	if (resource) {
		struct device *pdev;
		u32 gart_base, gart_acr;

		/* Remember this resource has been stored */
		resource->flags |= IORESOURCE_STORED;

		/* Find the size of the GART aperture */
		gart_acr = (0<<6)|(0<<5)|(0<<4)|((resource->gran - 25) << 1)|(0<<0);

		/* Get the base address */
		gart_base = ((resource->base) >> 25) & 0x00007fff;

		/* Update the other northbriges */
		pdev = 0;
		while ((pdev = dev_find_device(PCI_VENDOR_ID_AMD, pci_id, pdev))) {
			/* Store the GART size but don't enable it */
			pci_write_config32(pdev, 0x90, gart_acr);

			/* Store the GART base address */
			pci_write_config32(pdev, 0x94, gart_base);

			/* Don't set the GART Table base address */
			pci_write_config32(pdev, 0x98, 0);

			/* Report the resource has been stored... */
			report_resource_stored(pdev, resource, " <gart>");

			/* Errata 540 workaround */
			dword = pci_read_config32(pdev, 0x90);
			dword |= 0x1 << 6;			/* DisGartTblWlkPrb = 0x1 */
			pci_write_config32(pdev, 0x90, dword);
		}
	}
}

static void mcf3_set_resources_fam10h(struct device *dev)
{
	/* Set the gart aperture */
	set_agp_aperture(dev, 0x1203);

	/* Set the generic PCI resources */
	pci_dev_set_resources(dev);
}

static void mcf3_set_resources_fam15h_model10(struct device *dev)
{
	/* Set the gart aperture */
	set_agp_aperture(dev, 0x1403);

	/* Set the generic PCI resources */
	pci_dev_set_resources(dev);
}

static void mcf3_set_resources_fam15h(struct device *dev)
{
	/* Set the gart aperture */
	set_agp_aperture(dev, 0x1603);

	/* Set the generic PCI resources */
	pci_dev_set_resources(dev);
}

static void misc_control_init(struct device *dev)
{
	uint32_t dword;
	uint8_t nvram;
	uint8_t boost_limit;
	uint8_t current_boost;

	printk(BIOS_DEBUG, "NB: Function 3 Misc Control.. ");

#if IS_ENABLED(CONFIG_DIMM_DDR3) && !IS_ENABLED(CONFIG_NORTHBRIDGE_AMD_AGESA)
	uint8_t node;
	uint8_t slot;
	uint8_t dimm_present;

	/* Restore DRAM MCA registers */
	struct amdmct_memory_info *mem_info;
	mem_info = cbmem_find(CBMEM_ID_AMDMCT_MEMINFO);
	if (mem_info) {
		node = PCI_SLOT(dev->path.pci.devfn) - 0x18;

		/* Check node for installed DIMMs */
		dimm_present = 0;

		/* Check all slots for installed DIMMs */
		for (slot = 0; slot < MAX_DIMMS_SUPPORTED; slot++) {
			if (mem_info->dct_stat[node].DIMMPresent & (1 << slot)) {
				dimm_present = 1;
				break;
			}
		}

		if (dimm_present) {
			uint32_t mc4_status_high = pci_read_config32(dev, 0x4c);
			uint32_t mc4_status_low = pci_read_config32(dev, 0x48);
			if ((mc4_status_high & (0x1 << 31)) && (mc4_status_high != 0xffffffff)) {
				printk(BIOS_WARNING, "\nWARNING: MC4 Machine Check Exception detected on node %d!\n"
					"Signature: %08x%08x\n", node, mc4_status_high, mc4_status_low);
			}

			/* Clear MC4 error status */
			pci_write_config32(dev, 0x48, 0x0);
			pci_write_config32(dev, 0x4c, 0x0);
		}
	}
#endif

	/* Disable Machine checks from Invalid Locations.
	 * This is needed for PC backwards compatibility.
	 */
	dword = pci_read_config32(dev, 0x44);
	dword |= (1<<6) | (1<<25);
	pci_write_config32(dev, 0x44, dword);

	boost_limit = 0xf;
	if (get_option(&nvram, "maximum_p_state_limit") == CB_SUCCESS)
		boost_limit = nvram & 0xf;

	/* Set P-state maximum value */
	dword = pci_read_config32(dev, 0xdc);
	current_boost = (dword >> 8) & 0x7;
	if (boost_limit > current_boost)
		boost_limit = current_boost;
	dword &= ~(0x7 << 8);
	dword |= (boost_limit & 0x7) << 8;
	pci_write_config32(dev, 0xdc, dword);

	printk(BIOS_DEBUG, "done.\n");
}


static struct device_operations mcf3_ops_fam10h  = {
	.read_resources   = mcf3_read_resources,
	.set_resources    = mcf3_set_resources_fam10h,
	.enable_resources = pci_dev_enable_resources,
	.init             = misc_control_init,
	.scan_bus         = 0,
	.ops_pci          = 0,
};

static struct device_operations mcf3_ops_fam15h_model10  = {
	.read_resources   = mcf3_read_resources,
	.set_resources    = mcf3_set_resources_fam15h_model10,
	.enable_resources = pci_dev_enable_resources,
	.init             = misc_control_init,
	.scan_bus         = 0,
	.ops_pci          = 0,
};

static struct device_operations mcf3_ops_fam15h  = {
	.read_resources   = mcf3_read_resources,
	.set_resources    = mcf3_set_resources_fam15h,
	.enable_resources = pci_dev_enable_resources,
	.init             = misc_control_init,
	.scan_bus         = 0,
	.ops_pci          = 0,
};

static const struct pci_driver mcf3_driver __pci_driver = {
	.ops    = &mcf3_ops_fam10h,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1203,
};

static const struct pci_driver mcf3_driver_fam15_model10 __pci_driver = {
	.ops    = &mcf3_ops_fam15h_model10,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1403,
};

static const struct pci_driver mcf3_driver_fam15 __pci_driver = {
	.ops    = &mcf3_ops_fam15h,
	.vendor = PCI_VENDOR_ID_AMD,
	.device = 0x1603,
};
