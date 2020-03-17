/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <ec/google/wilco/romstage.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>
#include <baseboard/variants.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	struct cnl_mb_cfg board_memcfg;

	variant_mainboard_post_init_params(memupd);

	wilco_ec_romstage_init();

	cannonlake_memcfg_init(&memupd->FspmConfig, get_variant_memory_cfg(&board_memcfg));
}
