/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <fsp/api.h>
#include <gpio.h>
#include <soc/romstage.h>
#include <string.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();
	struct mem_spd spd_info;
	bool dimms_changed = false;

	memset(&spd_info, 0, sizeof(spd_info));
	variant_get_spd_info(&spd_info);

	const struct pad_config *pads;
	size_t pads_num;

	memcfg_init(memupd, mem_config, &spd_info, half_populated, &dimms_changed);
	if (dimms_changed) {
		memupd->FspmArchUpd.NvsBufferPtr = 0;
		memupd->FspmArchUpd.BootMode = FSP_BOOT_WITH_FULL_CONFIGURATION;
	}

	pads = variant_romstage_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);
}
