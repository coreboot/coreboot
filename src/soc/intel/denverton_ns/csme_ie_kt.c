/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 - 2017 Intel Corporation.
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
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <soc/pci_devs.h>
#include <soc/ramstage.h>

/**
* Read the base address registers for a given device.
*
* @param dev Pointer to the dev structure.
* @param howmany How many registers to read.
*/
static void pci_read_bases(struct device *dev, unsigned int howmany)
{
	unsigned long index;

	for (index = PCI_BASE_ADDRESS_0;
	     (index < PCI_BASE_ADDRESS_0 + (howmany << 2));) {
		struct resource *resource;
		resource = pci_get_resource(dev, index);
		/**
		* Workaround for Denverton-NS silicon (Rev A0/A1 for CSME/IE,
		*  Rev B0 for CSME only)
		*  CSME&IEs KT IO bar must be 16-byte aligned
		*/
		if ((resource->flags & IORESOURCE_IO) &&
		    (resource->align != 4)) {
			printk(BIOS_DEBUG,
			       "CSME&IEs KT IO bar must be 16-byte aligned!\n");
			resource->align = 4;
			resource->gran = 4;
			resource->size = 16;
		}
		index += (resource->flags & IORESOURCE_PCI64) ? 8 : 4;
	}

	compact_resources(dev);
}

static void pci_csme_ie_kt_read_resources(struct device *dev)
{
	/**
	* CSME/IE KT has 2 BARs to check:
	*   0x10 - KT IO BAR
	*   0x14 - KT Memory BAR
	* CSME/IE KT has no Expansion ROM BAR to check:
	*   0x30 - KT Host XRBAR, READ ONLY
	*/
	pci_read_bases(dev, 2);
}

static struct device_operations csme_ie_kt_ops = {
	.read_resources = pci_csme_ie_kt_read_resources,
	.set_resources = pci_dev_set_resources,
	.enable_resources = pci_dev_enable_resources,
	.scan_bus = 0,
	.init = 0,
	.ops_pci = &soc_pci_ops,
};

static const unsigned short pci_device_ids[] = {
	ME_MEKT_DEVID, /* DVN CSME KT */
	IE_MEKT_DEVID, /* DVN IE KT */
	0
};

static const struct pci_driver csme_ie_kt __pci_driver = {
	.ops = &csme_ie_kt_ops,
	.vendor = PCI_VENDOR_ID_INTEL,
	.devices = pci_device_ids,
};
