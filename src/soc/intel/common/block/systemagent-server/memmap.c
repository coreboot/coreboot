/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <intelblocks/systemagent_server.h>

void smm_region(uintptr_t *start, size_t *size)
{
	*start = sa_server_get_tseg_base();
	*size = sa_server_get_tseg_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	/*
	 * `postcar_mtrr_setup()` will be skipped if using FSP to tear down CAR.
	 */
	if (!CONFIG(NO_FSP_TEMP_RAM_EXIT))
		return;

	/*
	 * Add MTRR for ramstage, TSEG and extended BIOS region if exiting CAR without FSP is supporated.
	 */
	_Static_assert(!CONFIG(NO_FSP_TEMP_RAM_EXIT),
		       "NO_FSP_TEMP_RAM_EXIT is not supportted currently!");
}
