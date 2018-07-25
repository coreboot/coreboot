/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 * Copyright (C) 2017-2018 Intel Corp.
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
		.spd_smbus_address[0] = 0xA0,
		.spd_smbus_address[1] = 0xA2,
		.spd_smbus_address[2] = 0xA4,
		.spd_smbus_address[3] = 0xA6,
	};

	cannonlake_memcfg_init(&memupd->FspmConfig,
				variant_memcfg_config(), &spd);
}
