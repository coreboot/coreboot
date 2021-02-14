/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <ec/google/chromeec/ec.h>
#include <gpio.h>
#include <memory_info.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <variant/gpio.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct mb_cfg *board_cfg = variant_memcfg_config();
	const struct spd_info spd_info = {
		.read_type = READ_SPD_CBFS,
		.spd_spec.spd_index = variant_memory_sku(),
	};
	bool half_populated = variant_mem_is_half_populated();

	memcfg_init(&memupd->FspmConfig, board_cfg, &spd_info, half_populated);
}
