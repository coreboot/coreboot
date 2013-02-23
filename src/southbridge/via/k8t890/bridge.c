/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2011 Alexandru Gagniuc <mr.nuke.me@gmail.com>
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include "k8x8xx.h"

static void bridge_enable(struct device *dev)
{
	u8 tmp;
	print_debug("B188 device dump\n");

	/* VIA recommends this, sorry no known info. */

	writeback(dev, 0x40, 0x91);
	writeback(dev, 0x41, 0x40);
	writeback(dev, 0x43, 0x44);
#if CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800_OLD
	writeback(dev, 0x42, 0x80);
	writeback(dev, 0x44, 0x35);
#else
	writeback(dev, 0x44, 0x31); 	/* K8M890 should have 0x35 datasheet
					 * says it is reserved
					 */
#endif
	writeback(dev, 0x45, 0x3a);
	writeback(dev, 0x46, 0x88);	/* PCI ID lo */
	writeback(dev, 0x47, 0xb1);	/* PCI ID hi */

	/* Bridge control, K8M890 bit 3 should be set to enable VGA on AGP
	 * (Forward VGA compatible memory and I/O cycles )
	 */

#if CONFIG_SOUTHBRIDGE_VIA_SUBTYPE_K8T800_OLD
	writeback(dev, 0x3e, 0x0a);
#else
	writeback(dev, 0x3e, 0x16);
#endif
	dump_south(dev);

	/* disable I/O and memory decode, or it freezes PCI bus during BAR sizing */
	tmp = pci_read_config8(dev, PCI_COMMAND);
	tmp &= ~0x3;
	pci_write_config8(dev, PCI_COMMAND, tmp);

}

static const struct device_operations bridge_ops = {
	.read_resources		= pci_bus_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_bus_enable_resources,
	.enable			= bridge_enable,
	.scan_bus		= pci_scan_bridge,
	.reset_bus		= pci_bus_reset,
	.ops_pci		= 0,
};

/*
 * K8T890 and K8T800 both use device B188 as the bridge, so no need to
 * add a separate driver
 */
static const struct pci_driver northbridge_driver_890 __pci_driver = {
	.ops	= &bridge_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_BR,
};
