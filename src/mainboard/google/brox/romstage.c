/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <fsp/api.h>
#include <gpio.h>
#include <soc/romstage.h>
#include <string.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();
	struct mem_spd spd_info;

	memset(&spd_info, 0, sizeof(spd_info));
	variant_get_spd_info(&spd_info);

	const struct pad_config *pads;
	size_t pads_num;

	/* If battery is not present - Boot with maximum non-turbo frequency */
	if (CONFIG(EC_GOOGLE_CHROMEEC) && !google_chromeec_is_battery_present()) {
		FSP_M_CONFIG *mem_cfg = &memupd->FspmConfig;
		mem_cfg->BootFrequency = MAX_NONTURBO_PERFORMANCE;
		printk(BIOS_DEBUG, "Boot Frequency is set to %u\n", mem_cfg->BootFrequency);
	}

	memcfg_init(memupd, mem_config, &spd_info, half_populated);

	pads = variant_romstage_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);
}
