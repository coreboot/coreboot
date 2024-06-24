/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <fsp/api.h>
#include <soc/romstage.h>
#include <string.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct pad_config *pads;
	size_t pads_num;
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();
	struct mem_spd spd_info;

	pads = variant_romstage_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);

	memset(&spd_info, 0, sizeof(spd_info));
	variant_get_spd_info(&spd_info);

	memcfg_init(memupd, mem_config, &spd_info, half_populated);
}
