/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/auron/variant.h>
#include <soc/pei_data.h>
#include <soc/pei_wrapper.h>
#include <string.h>
#include <types.h>

/* Copy SPD data for on-board memory */
void mainboard_fill_spd_data(struct pei_data *pei_data)
{
	const unsigned int spd_index = variant_get_spd_index();

	fill_spd_for_index(pei_data->spd_data[0][0], spd_index);
	pei_data->spd_addresses[0] = SPD_MEMORY_DOWN;

	if (variant_is_dual_channel(spd_index)) {
		memcpy(pei_data->spd_data[1][0], pei_data->spd_data[0][0], SPD_LEN);
		pei_data->spd_addresses[2] = SPD_MEMORY_DOWN;
	}
}
