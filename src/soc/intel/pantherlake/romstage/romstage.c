/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbmem.h>
#include <console/console.h>
#include <fsp/api.h>
#include <intelblocks/cfg.h>
#include <intelblocks/cse.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/smbus.h>
#include <intelblocks/thermal.h>
#include <soc/intel/common/reset.h>
#include <soc/iomap.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <string.h>
#include <timestamp.h>
#include <ec/google/chromeec/ec.h>

void platform_fill_dimm_info_args(const DIMM_INFO *src_dimm,
	    const MEMORY_INFO_DATA_HOB *meminfo_hob,
	    struct dimm_fill_args *args)
{
	args->data_width = src_dimm->DataWidth;
	args->mfg_id_arg = src_dimm->MfgId.Data;
}

/*
 * Checks if the Converged Security Engine (CSE) is in a malicious state.
 *
 * A malicious state is inferred when the CSE has been temporarily disabled via
 * software (HFS1 COM Soft Temp Disable) but the Current Operating State (COS)
 * is not in default aka pre-boot state.
 *
 * @return true if the CSE is in a malicious state, false otherwise.
 */
static bool cse_in_malicious_state(void)
{
	return cse_is_hfs1_com_soft_temp_disable() && !cse_is_hfs1_cos_default();
}

static void do_board_reset(void)
{
	if (CONFIG(EC_GOOGLE_CHROMEEC))
		google_chromeec_reboot(EC_REBOOT_COLD, EC_REBOOT_FLAG_IMMEDIATE);
}

void mainboard_romstage_entry(void)
{
	struct chipset_power_state *ps = pmc_get_power_state();
	bool s3wake = pmc_fill_power_state(ps) == ACPI_S3;

	/* Initialize HECI interface */
	cse_init(HECI1_BASE_ADDRESS);

	if (cse_in_malicious_state()) {
		printk(BIOS_INFO, "CSE is in malicious state inside %s(), "
		       "doing board reset\n", __func__);
		do_board_reset();
	}

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
		fsp_save_dimm_info();
}
