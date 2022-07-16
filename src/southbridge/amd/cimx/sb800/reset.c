/* SPDX-License-Identifier: GPL-2.0-only */

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <device/pci_ops.h>
#include <cf9_reset.h>
#include <reset.h>

#define HT_INIT_CONTROL		0x6c
#define HTIC_BIOSR_Detect	(1 << 5)

#define DEV_CDB 0x18
#define NODE_PCI(x, fn)	(((DEV_CDB + x) < 32) ? (PCI_DEV(0, (DEV_CDB + x), fn)) : (PCI_DEV((0 - 1), (DEV_CDB + x - 32), fn)))

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
