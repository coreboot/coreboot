/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>

void bootblock_early_southbridge_init(void)
{
	/* Set FWH IDs for 2 MB flash part. */
	if (CONFIG_ROM_SIZE == 0x200000)
		pci_write_config32(PCI_DEV(0, 0x1f, 0), 0xe8, 0x00001111);
}
