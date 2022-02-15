/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/memmap.h>
#include <amdblocks/pmlib.h>
#include <arch/cpu.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <fsp/api.h>
#include <program_loading.h>
#include <timestamp.h>
#include <types.h>

asmlinkage void car_stage_entry(void)
{
	timestamp_add_now(TS_ROMSTAGE_START);

	post_code(0x40);

	console_init();

	post_code(0x42);

	/* Snapshot chipset state prior to any FSP call. */
	fill_chipset_state();

	post_code(0x43);
	fsp_memory_init(acpi_is_wakeup_s3());

	memmap_stash_early_dram_usage();

	post_code(0x44);
	run_ramstage();

	post_code(0x50); /* Should never see this post code. */
}
