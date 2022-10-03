/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <southbridge/intel/common/early_spi.h>
#include <southbridge/intel/common/rcba.h>

#include "i82801gx.h"

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetching_and_caching();

	i82801gx_setup_bars();

	/* Enable upper 128bytes of CMOS */
	RCBA32(0x3400) = (1 << 2);

	/* Disable watchdog timer */
	RCBA32(GCS) = RCBA32(GCS) | 0x20;

	i82801gx_lpc_setup();
}
