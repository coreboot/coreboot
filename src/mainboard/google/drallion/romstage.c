/* SPDX-License-Identifier: GPL-2.0-only */

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
