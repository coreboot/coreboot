/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <baseboard/variants.h>
#include <soc/meminit.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	meminit_lpddr4_by_sku(&memupd->FspmConfig,
		variant_lpddr4_config(), variant_memory_sku());
}

void mainboard_save_dimm_info(void)
{
	save_lpddr4_dimm_info(variant_lpddr4_config(), variant_memory_sku());
}
