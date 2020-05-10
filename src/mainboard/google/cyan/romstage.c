/* SPDX-License-Identifier: GPL-2.0-only */

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
