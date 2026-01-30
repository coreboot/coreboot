/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <baseboard/variants.h>
#include <bootmode.h>
#include <ec/google/chromeec/ec.h>
#include <soc/romstage.h>
#include <soc/soc_chip.h>
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
}

void platform_romstage_pre_mem(void)
{
	/*
	 * Only alert the user (set LED to red in color) if the lid is closed and the battery
	 * is critically low without AC power.
	 */
	if (CONFIG(EC_GOOGLE_CHROMEEC) && CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch() &&
			 google_chromeec_is_critically_low_on_battery())
		google_chromeec_set_lightbar_rgb(0xff, 0xff, 0x00, 0x00);
}
