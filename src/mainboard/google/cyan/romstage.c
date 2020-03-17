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

#include <soc/romstage.h>
#include <baseboard/variants.h>

#include "spd/spd_util.h"

void mainboard_memory_init_params(struct romstage_params *params,
	MEMORY_INIT_UPD *memory_params)
{
	spd_memory_init_params(memory_params);

	/* Variant-specific memory params */
	variant_memory_init_params(memory_params);
}

__weak
void variant_memory_init_params(MEMORY_INIT_UPD *memory_params)
{
}
