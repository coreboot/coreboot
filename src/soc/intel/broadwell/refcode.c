/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <acpi/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <console/streams.h>
#include <program_loading.h>
#include <rmodule.h>
#include <stage_cache.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <soc/pm.h>
#include <soc/refcode.h>

static pei_wrapper_entry_t load_reference_code(void)
{
	if (resume_from_stage_cache()) {
		struct prog prog;
		stage_cache_load_stage(STAGE_REFCODE, &prog);
		return prog_entry(&prog);
	}

	struct prog prog =
		PROG_INIT(PROG_REFCODE, CONFIG_CBFS_PREFIX "/refcode");
	struct rmod_stage_load refcode = {
		.cbmem_id = CBMEM_ID_REFCODE,
		.prog = &prog,
	};

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
	pei_data.saved_data = (void *)&dummy;

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
