/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <amdblocks/acpi.h>
#include <amdblocks/apob_cache.h>
#include <amdblocks/memmap.h>
#include <arch/cpu.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <fsp/api.h>
#include <program_loading.h>
#include <soc/acpi.h>
#include <types.h>

static struct chipset_power_state chipset_state;

static void fill_chipset_state(void)
{
	acpi_fill_pm_gpe_state(&chipset_state.gpe_state);
	gpio_fill_wake_state(&chipset_state.gpio_state);
}

static void add_chipset_state_cbmem(int unused)
{
	struct chipset_power_state *state;

	state = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*state));

	if (state)
		memcpy(state, &chipset_state, sizeof(*state));
}

ROMSTAGE_CBMEM_INIT_HOOK(add_chipset_state_cbmem);

asmlinkage void car_stage_entry(void)
{
	post_code(0x40);
	console_init();

	post_code(0x42);

	/* Snapshot chipset state prior to any FSP call. */
	fill_chipset_state();

	post_code(0x43);
	fsp_memory_init(acpi_is_wakeup_s3());
	soc_update_apob_cache();

	memmap_stash_early_dram_usage();

	post_code(0x44);
	run_ramstage();

	post_code(0x50); /* Should never see this post code. */
}
