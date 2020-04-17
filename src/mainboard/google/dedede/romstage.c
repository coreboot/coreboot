/*
 * This file is part of the coreboot project.
 *
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include <baseboard/variants.h>
#include <gpio.h>
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
	bool half_populated = !gpio_get(GPIO_MEM_CH_SEL);

	memcfg_init(&memupd->FspmConfig, board_cfg, &spd_info, half_populated);
}
