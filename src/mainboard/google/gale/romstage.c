/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/soc_services.h>
#include "mmu.h"

void main(void)
{
	console_init();
	initialize_dram();

	/* Add dram mappings to mmu tables. */
	setup_dram_mappings(DRAM_INITIALIZED);

	cbmem_initialize_empty();
	run_ramstage();
}
