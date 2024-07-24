/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/util.h>
#include <intelblocks/cfg.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/smbus.h>
#include <intelblocks/thermal.h>
#include <memory_info.h>
#include <soc/intel/common/smbios.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>
#include <timestamp.h>

/* Save the DIMM information for SMBIOS table 17 */
static void save_dimm_info(void)
{
	/* Update after FSP is released externally released. */
}

void mainboard_romstage_entry(void)
{
	struct chipset_power_state *ps = pmc_get_power_state();
	bool s3wake = pmc_fill_power_state(ps) == ACPI_S3;

	/* Initialize HECI interface */
	cse_init(HECI1_BASE_ADDRESS);

	if (!s3wake && CONFIG(SOC_INTEL_CSE_LITE_SKU)) {
		cse_fill_bp_info();
		if (CONFIG(SOC_INTEL_CSE_LITE_SYNC_IN_ROMSTAGE))
			cse_fw_sync();
	}

	/* Update coreboot timestamp table with CSE timestamps */
	if (CONFIG(SOC_INTEL_CSE_PRE_CPU_RESET_TELEMETRY))
		cse_get_telemetry_data();

	/* Program MCHBAR, DMIBAR, GDXBAR and EDRAMBAR */
	systemagent_early_init();
	/* Program SMBus base address and enable it */
	smbus_common_init();

	/*
	 * Set low maximum temp threshold value used for dynamic thermal sensor
	 * shutdown consideration.
	 *
	 * If Dynamic Thermal Shutdown is enabled then PMC logic shuts down the
	 * thermal sensor when CPU is in a C-state and LTT >= DTS Temp.
	 */
	pch_thermal_configuration();
	fsp_memory_init(s3wake);
	pmc_set_disb();
	if (!s3wake)
		save_dimm_info();
}
