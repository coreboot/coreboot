/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/fsp.h>
#include <amdblocks/post_codes.h>
#include <amdblocks/memmap.h>
#include <amdblocks/pmlib.h>
#include <amdblocks/stb.h>
#include <cbmem.h>
#include <program_loading.h>
#include <romstage_common.h>

void __noreturn romstage_main(void)
{
	post_code(POSTCODE_ROMSTAGE_MAIN);

	if (CONFIG(WRITE_STB_BUFFER_TO_CONSOLE))
		write_stb_to_console();

	if (CONFIG(SOC_AMD_COMMON_BLOCK_PM_CHIPSET_STATE_SAVE)) {
		/* Snapshot chipset state prior to any reference code call. */
		fill_chipset_state();
	}

	if (CONFIG(PLATFORM_USES_FSP2_0)) {
		amd_fsp_early_init();
	} else {
		cbmem_initialize_empty();
	}

	if (CONFIG(SOC_AMD_COMMON_ROMSTAGE_LEGACY_DMA_FIXUP)) {
		/* Fix up settings the reference code should not be changing */
		fch_disable_legacy_dma_io();
	}

	memmap_stash_early_dram_usage();

	run_ramstage();
}
