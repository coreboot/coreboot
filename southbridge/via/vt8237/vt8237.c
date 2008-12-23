/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Corey Osgood <corey.osgood@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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
#include <console.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <statictree.h>
#include <config.h>
#include "vt8237.h"

/*
 * Datasheet: http://www.via.com.tw/en/downloads/datasheets/chipsets/
 *		VT8237R_SouthBridge_Revision2.06_Lead-Free.zip
 */

void vt8237_enable(struct device *dev)
{
	struct device *lpc_dev;
	u16 sb_fn_ctrl;

	const u8 func = dev->path.pci.devfn & 0x7;
	const u8 device = dev->path.pci.devfn >> 3;
	const int d16_index[6] = {12, 13, 10, 8, 9, 7};


	printk(BIOS_DEBUG, "Enabling/Disabling device 0x%x function 0x%x.\n",
							device, func);

	if(dev->id.pci.vendor != PCI_VENDOR_ID_VIA)
		return;

	lpc_dev = dev_find_slot(0, PCI_BDF(0, 17, 0));
	sb_fn_ctrl = pci_read_config8(lpc_dev, 0x50) << 8;
	sb_fn_ctrl |= pci_read_config8(lpc_dev, 0x51);

	if (device == 16)
	{
		/* If any port is enabled, the first port needs to be enabled */
		if (dev->enabled)
		{
			sb_fn_ctrl &= ~(1 << d16_index[0]);
			sb_fn_ctrl &= ~(1 << d16_index[func]);
		}
		else
		{
			sb_fn_ctrl |= (1 << d16_index[func]);
		}
	}
	else if (device == 17)
	{
		if (func == 5)
		{
			sb_fn_ctrl &= ~(dev->enabled << 14);
			sb_fn_ctrl |= (!dev->enabled << 14); 
		}
		else if (func == 6)
		{
			sb_fn_ctrl &= ~(dev->enabled << 15);
			sb_fn_ctrl |= (!dev->enabled << 15);
		}
	}
			
	pci_write_config8(dev, 0x50, (sb_fn_ctrl >> 8) & 0xff);
	pci_write_config8(dev, 0x51, sb_fn_ctrl & 0xff);

	/* TODO: If SATA is disabled, move IDE to fn0 to conform PCI specs. */
}
