/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/memmap.h>
#include <amdblocks/pmlib.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <fsp/api.h>
#include <program_loading.h>
#include <romstage_common.h>
#include <types.h>

void __noreturn romstage_main(void)
{
	post_code(0x40);

	/* Snapshot chipset state prior to any FSP call. */
	fill_chipset_state();

	post_code(0x43);
	fsp_memory_init(acpi_is_wakeup_s3());

	memmap_stash_early_dram_usage();

	post_code(0x44);
	run_ramstage();
}
