/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/apob_cache.h>
#include <amdblocks/memmap.h>
#include <amdblocks/pmlib.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <fsp/api.h>
#include <program_loading.h>
#include <security/vboot/vboot_common.h>

asmlinkage void car_stage_entry(void)
{
	post_code(0x40);
	console_init();

	if (CONFIG(VBOOT_EARLY_EC_SYNC))
		vboot_sync_ec();

	post_code(0x41);

	/* Snapshot chipset state prior to any FSP call */
	fill_chipset_state();

	fsp_memory_init(acpi_is_wakeup_s3());
	soc_update_apob_cache();

	/* Fixup settings FSP-M should not be changing */
	fch_disable_legacy_dma_io();

	memmap_stash_early_dram_usage();

	run_ramstage();
}
