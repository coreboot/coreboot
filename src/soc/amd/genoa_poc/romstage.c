/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/post_codes.h>
#include <amdblocks/memmap.h>
#include <cbmem.h>
#include <program_loading.h>
#include <romstage_common.h>

void __noreturn romstage_main(void)
{
	post_code(POSTCODE_ROMSTAGE_MAIN);

	cbmem_initialize_empty();
	memmap_stash_early_dram_usage();
	run_ramstage();
}
