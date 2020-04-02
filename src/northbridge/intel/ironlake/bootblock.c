/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/bootblock.h>
#include <device/pci_ops.h>

void bootblock_early_northbridge_init(void)
{
	pci_io_write_config32(PCI_DEV(0xff, 0x00, 1), 0x50, CONFIG_MMCONF_BASE_ADDRESS | 1);
	pci_io_write_config32(PCI_DEV(0xff, 0x00, 1), 0x54, 0);
}
