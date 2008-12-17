/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 Corey Osgood <corey.osgood@gmail.com>
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
 */

#include <types.h>
#include <lib.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "cn700.h"

static void memctrl_init(struct device *dev)
{
	struct device *vlink_dev;
	u16 reg16;
	u8 ranks, pagec, paged, pagee, pagef, shadowreg;

	/* Set up the VGA framebuffer size. */
	reg16 = (log2f(CONFIG_CN700_VIDEO_MB_32) << 12) | (1 << 15);
	pci_write_config16(dev, 0xa0, reg16);

	/* Set up VGA timers. */
	pci_write_config8(dev, 0xa2, 0x44);

	for (ranks = 0x4b; ranks >= 0x48; ranks--) {
		if (pci_read_config8(dev, ranks)) {
			ranks -= 0x48;
			break;
		}
	}
	if (ranks == 0x47)
		ranks = 0x00;
	reg16 = 0xaae0;
	reg16 |= ranks;
	/* GMINT Misc. FrameBuffer rank */
	pci_write_config16(dev, 0xb0, reg16);
	/* AGPCINT Misc. */
	pci_write_config8(dev, 0xb8, 0x08);

	/* TODO: This doesn't belong here. At the very least make it a dts
	 * option */

#if 0	/* Handled in stage1 */
	/* Shadow RAM */
	pagec = 0xff, paged = 0xff, pagee = 0xff, pagef = 0x30;
	/* PAGE C, D, E are all read write enable */
	pci_write_config8(dev, 0x80, pagec);
	pci_write_config8(dev, 0x81, paged);
	pci_write_config8(dev, 0x82, pagee);
	/* PAGE F are read/writable */
	shadowreg = pci_read_config8(dev, 0x83);
	shadowreg |= pagef;
	pci_write_config8(dev, 0x83, shadowreg);
	/* vlink mirror */
	vlink_dev = dev_find_slot(0, PCI_BDF(0, 7, 0))
	pci_write_config8(vlink_dev, 0x61, pagec);
	pci_write_config8(vlink_dev, 0x62, paged);
	pci_write_config8(vlink_dev, 0x64, pagee);
	shadowreg = pci_read_config8(vlink_dev, 0x63);
	shadowreg |= pagef;
	pci_write_config8(vlink_dev, 0x63, shadowreg);
#endif
}

struct device_operations cn700_memctrl = {
	.id = {.type = DEVICE_ID_PCI,
		{.pci = {.vendor = PCI_VENDOR_ID_VIA,
				.device = PCI_DEVICE_ID_VIA_CN700_MEMCTRL}}},
	.constructor			= default_device_constructor,
	//.phase3_scan			= scan_static_bus,
	.phase4_read_resources		= pci_dev_read_resources,
	.phase4_set_resources		= pci_set_resources,
	.phase5_enable_resources	= pci_dev_enable_resources,
	.phase6_init			= memctrl_init,
	.ops_pci		 	= &pci_dev_ops_pci,
};
