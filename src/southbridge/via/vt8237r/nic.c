/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007, 2008 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 Jon Harrison <bothlyn@blueyonder.co.uk>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include "vt8237r.h"


static void vt8237_eth_read_resources(struct device *dev)
{
#if CONFIG_EPIA_VT8237R_INIT
	struct resource *res;

	/* Fix the I/O Resources of the USB2.0 Interface */
	res = new_resource(dev, PCI_BASE_ADDRESS_0);
	res->base = 0xF6001000ULL;
	res->size = 256;
	res->align = 12;
	res->gran = 8;
	res->limit = res->base + res->size - 1;
	res->flags = IORESOURCE_MEM | IORESOURCE_FIXED |
		     		IORESOURCE_ASSIGNED;
#else
	pci_dev_read_resources(dev);
#endif
	return;
}


static const struct device_operations vt8237_eth_ops = {
	.read_resources		= vt8237_eth_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.init				= 0,
	.enable				= 0,
	.ops_pci			= 0,
};

static const struct pci_driver vt8237r_driver_eth __pci_driver = {
	.ops	= &vt8237_eth_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_8233_7,
};
