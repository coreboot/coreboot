/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <arch/romstage.h>

#include <southbridge/intel/i82801dx/i82801dx.h>
#include <northbridge/intel/e7505/raminit.h>

void mainboard_romstage_entry(void)
{
	/* FIXME: Keep until flashed bootblock has these. */
	i82801dx_early_init();
	i82801dx_lpc_setup();

	sdram_initialize();

	cbmem_recovery(0);
}
