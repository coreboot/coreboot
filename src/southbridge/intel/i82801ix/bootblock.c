/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include "i82801ix.h"

static void enable_spi_prefetch(void)
{
	pci_update_config8(PCI_DEV(0, 0x1f, 0), 0xdc, ~(3 << 2), 2 << 2);
}

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetch();

	i82801ix_early_init();
	i82801ix_lpc_setup();
}
