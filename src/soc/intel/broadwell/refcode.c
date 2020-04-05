/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <string.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/streams.h>
#include <cpu/x86/tsc.h>
#include <program_loading.h>
#include <rmodule.h>
#include <stage_cache.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/ramstage.h>

static pei_wrapper_entry_t load_refcode_from_cache(void)
{
	struct prog refcode;

	printk(BIOS_DEBUG, "refcode loading from cache.\n");

	stage_cache_load_stage(STAGE_REFCODE, &refcode);

	return (pei_wrapper_entry_t)prog_entry(&refcode);
}

static pei_wrapper_entry_t load_reference_code(void)
{
	struct prog prog =
		PROG_INIT(PROG_REFCODE, CONFIG_CBFS_PREFIX "/refcode");
	struct rmod_stage_load refcode = {
		.cbmem_id = CBMEM_ID_REFCODE,
		.prog = &prog,
	};

	if (acpi_is_wakeup_s3())
		return load_refcode_from_cache();

	if (prog_locate(&prog)) {
		printk(BIOS_DEBUG, "Couldn't locate reference code.\n");
		return NULL;
	}

	if (rmodule_stage_load(&refcode)) {
		printk(BIOS_DEBUG, "Error loading reference code.\n");
		return NULL;
	}

	/* Cache loaded reference code. */
	stage_cache_add(STAGE_REFCODE, &prog);

	return (pei_wrapper_entry_t)prog_entry(&prog);
}

void broadwell_run_reference_code(void)
{
	int ret, dummy;
	struct pei_data pei_data;
	pei_wrapper_entry_t entry;

	memset(&pei_data, 0, sizeof(pei_data));
	mainboard_fill_pei_data(&pei_data);
	broadwell_fill_pei_data(&pei_data);

	pei_data.boot_mode = acpi_is_wakeup_s3() ? ACPI_S3 : 0;
	pei_data.saved_data = (void *) &dummy;

	entry = load_reference_code();
	if (entry == NULL) {
		printk(BIOS_ERR, "Reference code not found\n");
		return;
	}

	/* Call into reference code. */
	ret = entry(&pei_data);
	if (ret != 0) {
		printk(BIOS_ERR, "Reference code returned %d\n", ret);
		return;
	}
}
