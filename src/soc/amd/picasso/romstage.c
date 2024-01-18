/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/fsp.h>
#include <amdblocks/memmap.h>
#include <amdblocks/pmlib.h>
#include <amdblocks/post_codes.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <program_loading.h>
#include <romstage_common.h>
#include <types.h>

void __noreturn romstage_main(void)
{
	post_code(POSTCODE_ROMSTAGE_MAIN);

	/* Snapshot chipset state prior to any FSP call. */
	fill_chipset_state();

	amd_fsp_early_init();

	memmap_stash_early_dram_usage();

	run_ramstage();
}
