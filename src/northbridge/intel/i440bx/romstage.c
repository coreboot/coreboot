/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <cbmem.h>
#include <southbridge/intel/i82371eb/i82371eb.h>
#include <northbridge/intel/i440bx/raminit.h>

void mainboard_romstage_entry(void)
{
	i82371eb_early_init();

	sdram_initialize(0);
	cbmem_initialize_empty();
}
