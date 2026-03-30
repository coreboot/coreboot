/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/bootblock.h>
#include <southbridge/intel/common/early_spi.h>
#include "i82801hx.h"

void bootblock_early_southbridge_init(void)
{
	enable_spi_prefetching_and_caching();

	i82801hx_early_init();
	i82801hx_lpc_setup();
}
