/*
 * This file is part of i915tool
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include "video.h"

int find_idlist(struct drm_device *dev, u16 vendor, u16 device)
{
	extern const struct pci_device_id pciidlist[];
	int succ = 0;
	int i;
	for(i = 0; pciidlist[i].vendor && !succ; i++){
		if (vendor == pciidlist[i].vendor && device ==
				pciidlist[i].device){
			dev->dev_private->info =
				(void *)(pciidlist[i].driver_data);
			succ = 1;
		}
	}
	return succ;
}

/* there's only going to be one device ... */
int pci_dev_find(struct drm_device *dev)
{
	struct pci_access *pacc = NULL;
	struct pci_dev *temp;
	int succ = 0;
        pacc = pci_alloc();
	if (! pacc)
		return 0;
        pci_init(pacc);
        pci_scan_bus(pacc);

	for (temp = pacc->devices; temp && ! dev->pdev; temp = temp->next){
		if ((temp->device_class & 0xff00) == PCI_CLASS_DISPLAY_VGA ) {
			pci_fill_info(temp, PCI_FILL_IDENT |
					PCI_FILL_BASES | PCI_FILL_CLASS);
			dev->pdev = temp;
		}
	}

	if (dev->pdev)
		succ = find_idlist(dev, dev->pdev->vendor_id,
					dev->pdev->device_id);
	return succ;
}

/* Support library for kernel style pci functions. We could do a semantic
 * patch for it but this is easier to debug as we can fill it with prints
 * if we want. And no cpp abuse here. Keep it simple.
 */

void pci_read_config_byte(struct pci_dev *dev, unsigned long offset, u8 *val)
{
	*val = pci_read_byte(dev, offset);
}

void pci_write_config_byte(struct pci_dev *dev, unsigned long offset, u8 val)
{
	pci_write_byte(dev, offset, val);
}

void pci_read_config_word(struct pci_dev *dev, unsigned long offset, u16 *val)
{
	*val = pci_read_word(dev, offset);
}

void pci_write_config_word(struct pci_dev *dev, unsigned long offset, u16 val)
{
	pci_write_word(dev, offset, val);
}

void pci_read_config_dword(struct pci_dev *dev, unsigned long offset, u32 *val)
{
	*val = pci_read_long(dev, offset);
}

void pci_write_config_dword(struct pci_dev *dev, unsigned long offset, u32 val)
{
	pci_write_long(dev, offset, val);
}
