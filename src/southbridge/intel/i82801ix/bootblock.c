/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include <southbridge/intel/common/early_spi.h>
#include "i82801ix.h"

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetching_and_caching();

	i82801ix_early_init();
	i82801ix_lpc_setup();
}
