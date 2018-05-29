/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
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

#include <cbmem.h>
#include <console/console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <soc/iomap.h>
#include <soc/ramstage.h>

#define RES_IN_KIB(r) ((r) >> 10)

static void nc_read_resources(struct device *dev)
{
	unsigned long base_k;
	int index = 0;
	unsigned long size_k;

	/* Read standard PCI resources. */
	pci_dev_read_resources(dev);

	/* 0 -> 0xa0000 */
	base_k = 0;
	size_k = 0xa0000 - base_k;
	ram_resource(dev, index++, RES_IN_KIB(base_k), RES_IN_KIB(size_k));

	/*
	 * Reserve everything between A segment and 1MB:
	 *
	 * 0xa0000 - 0xbffff: legacy VGA
	 * 0xc0000 - 0xdffff: RAM
	 * 0xe0000 - 0xfffff: ROM shadow
	 */
	base_k += size_k;
	size_k = 0xc0000 - base_k;
	mmio_resource(dev, index++, RES_IN_KIB(base_k), RES_IN_KIB(size_k));

	base_k += size_k;
	size_k = 0x100000 - base_k;
	reserved_ram_resource(dev, index++, RES_IN_KIB(base_k),
		RES_IN_KIB(size_k));

	/* 0x100000 -> cbmem_top - cacheable and usable */
	base_k += size_k;
	size_k = (unsigned long)cbmem_top() - base_k;
	ram_resource(dev, index++, RES_IN_KIB(base_k), RES_IN_KIB(size_k));

	/* cbmem_top -> 0xc0000000 - reserved */
	base_k += size_k;
	size_k = 0xc0000000 - base_k;
	reserved_ram_resource(dev, index++, RES_IN_KIB(base_k),
		RES_IN_KIB(size_k));

	/* 0xc0000000 -> 4GiB is mmio. */
	base_k += size_k;
	size_k = 0x100000000ull - base_k;
	mmio_resource(dev, index++, RES_IN_KIB(base_k), RES_IN_KIB(size_k));
}

static struct device_operations nc_ops = {
	.read_resources   = &nc_read_resources,
	.set_resources    = &pci_dev_set_resources,
	.enable_resources = &pci_dev_enable_resources,
};

static const struct pci_driver systemagent_driver __pci_driver = {
	.ops	= &nc_ops,
	.vendor	= PCI_VENDOR_ID_INTEL,
	.device	= QUARK_MC_DEVICE_ID
};
