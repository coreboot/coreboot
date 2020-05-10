/* SPDX-License-Identifier: GPL-2.0-only */

#include <fsp/car.h>
#include <program_loading.h>

void *cache_as_ram_stage_main(FSP_INFO_HEADER *fih)
{
	run_romstage();
	/* Will actually never return. */
	return NULL;
}
