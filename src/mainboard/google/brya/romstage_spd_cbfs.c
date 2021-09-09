/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <fsp/api.h>
#include <gpio.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSP_M_CONFIG *m_cfg)
{
	const struct mb_cfg *mem_config = variant_memory_params();
	bool half_populated = variant_is_half_populated();
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = variant_memory_sku(),
	};
	const struct pad_config *pads;
	size_t pads_num;

	memcfg_init(m_cfg, mem_config, &spd_info, half_populated);

	pads = variant_romstage_gpio_table(&pads_num);
	gpio_configure_pads(pads, pads_num);
}
