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
	const struct ddr_memory_cfg *board_cfg = variant_memory_params();
	const struct spd_info spd_info = {
		.topology = MEMORY_DOWN,
		.md_spd_loc = SPD_CBFS,
		.cbfs_index = variant_memory_sku(),
	};
	bool half_populated = gpio_get(GPIO_MEM_CH_SEL);

	meminit_ddr(mem_cfg, board_cfg, &spd_info, half_populated);
}
