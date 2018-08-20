/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
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

#include <baseboard/variants.h>
#include <soc/cnl_memcfg_init.h>
#include <soc/romstage.h>

void mainboard_memory_init_params(FSPM_UPD *memupd)
{
	const struct spd_info spd = {
		.spd_by_index = true,
		.spd_spec.spd_index = variant_memory_sku(),
	};

	cannonlake_memcfg_init(&memupd->FspmConfig,
				variant_lpddr4_config(), &spd);
}
