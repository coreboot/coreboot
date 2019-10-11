/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Facebook Inc.
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

#define __SIMPLE_DEVICE__

#include <stdint.h>
#include <soc/ubox.h>

void iio_hide(DEVTREE_CONST struct device *dev)
{
	pci_devfn_t ubox_dev;
	uint8_t slot, func;

	slot = PCI_SLOT(dev->path.pci.devfn);
	func = PCI_FUNC(dev->path.pci.devfn);
	ubox_dev = PCI_DEV(get_busno1(), UBOX_DEV, UBOX_FUNC);
	pci_or_config32(ubox_dev, UBOX_DEVHIDE0 + func * 4, 1 << slot);
}
