/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <fsp/soc_binding.h>
#include <fw_config.h>
#include <gpio.h>
#include <memory_info.h>
#include <soc/gpio.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <variant/gpio.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	FSP_M_CONFIG *mem_cfg = &mupd->FspmConfig;
	const struct mb_cfg *board_cfg = variant_memory_params();
	const struct mem_spd spd_info = {
		.topo = MEM_TOPO_MEMORY_DOWN,
		.cbfs_index = variant_memory_sku(),
	};
	bool half_populated = gpio_get(GPIO_MEM_CH_SEL);

	memcfg_init(mem_cfg, board_cfg, &spd_info, half_populated);
	memcfg_variant_init(mupd);
}

void __weak memcfg_variant_init(FSPM_UPD *mupd)
{
}
