/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <device/pci_ops.h>
#include <southbridge/intel/common/early_spi.h>
#include "i82801jx.h"

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetching_and_caching();

	i82801jx_setup_bars();

	/* Enable upper 128bytes of CMOS. */
	RCBA32(0x3400) = (1 << 2);

	i82801jx_lpc_setup();
}
