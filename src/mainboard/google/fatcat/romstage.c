/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <baseboard/variants.h>
#include <bootmode.h>
#include <ec/google/chromeec/ec.h>
#include <fsp/api.h>
#include <security/vboot/vboot_common.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
#include <static.h>
#include <string.h>

/*
 * Placeholder to configure GPIO early from romstage relying on the FW_CONFIG.
 *
 * If any platform would like to override early GPIOs, they should override from
 * the variant directory.
 */
__weak void fw_config_configure_pre_mem_gpio(void)
{
	/* Nothing to do */
}

__weak void variant_update_soc_memory_init_params(FSPM_UPD *memupd)
{
	/* Nothing to do */
}

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	size_t pads_num;
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();
	struct mem_spd spd_info;

	pads = variant_romstage_gpio_table(&pads_num);
	if (pads_num)
		gpio_configure_pads(pads, pads_num);
	fw_config_configure_pre_mem_gpio();

	memset(&spd_info, 0, sizeof(spd_info));
	variant_get_spd_info(&spd_info);

	memcfg_init(memupd, mem_config, &spd_info, half_populated);

	/* Override FSP-M UPD per board if required. */
	variant_update_soc_memory_init_params(memupd);

	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	/* Disable CPU ratio override for unstable power scenarios */
	if (!google_chromeec_is_battery_present() ||
			 google_chromeec_is_below_critical_threshold()) {
		const struct soc_intel_pantherlake_config *config = config_of_soc();
		FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;
		if (config->cpu_ratio_override)
			m_cfg->CpuRatio = 0;
	}
}

void platform_romstage_pre_mem(void)
{
	/*
	 * Early initialization of the Chrome EC lightbar.
	 * Ensures visual continuity if the AP firmware disabled the lightbar
	 * in a previous boot without a subsequent EC reset.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC_LED_CONTROL))
		google_chromeec_lightbar_on();
}

bool mainboard_can_allow_flex_ratio_override(void)
{
    if (!CONFIG(VBOOT))
        return false;

    if (vboot_recovery_mode_enabled() || vboot_check_recovery_request())
        return false;

    return true;
}
