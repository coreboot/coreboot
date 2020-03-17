/*
 * This file is part of the coreboot project.
 *
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

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <cf9_reset.h>
#include <reset.h>

#define HT_INIT_CONTROL		0x6C
#define HTIC_BIOSR_Detect	(1<<5)

#define DEV_CDB 0x18
#define NODE_PCI(x, fn)	(((DEV_CDB+x)<32)?(PCI_DEV(0,(DEV_CDB+x),fn)):(PCI_DEV((0-1),(DEV_CDB+x-32),fn)))

void cf9_reset_prepare(void)
{
	u32 nodes;
	u32 htic;
	pci_devfn_t dev;
	int i;

	nodes = ((pci_read_config32(PCI_DEV(0, DEV_CDB, 0), 0x60) >> 4) & 7) + 1;
	for (i = 0; i < nodes; i++) {
		dev = NODE_PCI(i, 0);
		htic = pci_read_config32(dev, HT_INIT_CONTROL);
		htic &= ~HTIC_BIOSR_Detect;
		pci_write_config32(dev, HT_INIT_CONTROL, htic);
	}
}

void do_board_reset(void)
{
	system_reset();
}
