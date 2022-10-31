/* SPDX-License-Identifier: GPL-2.0-only */

/* TODO: Check if this is still correct */

#include <acpi/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/memmap.h>
#include <amdblocks/pmlib.h>
#include <amdblocks/stb.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <fsp/api.h>
#include <program_loading.h>
#include <romstage_common.h>

void __noreturn romstage_main(void)
{
	post_code(0x40);

	if (CONFIG(WRITE_STB_BUFFER_TO_CONSOLE))
		write_stb_to_console();

	/* Snapshot chipset state prior to any FSP call */
	fill_chipset_state();

	fsp_memory_init(acpi_is_wakeup_s3());

	/* Fixup settings FSP-M should not be changing */
	fch_disable_legacy_dma_io();

	memmap_stash_early_dram_usage();

	run_ramstage();
}
