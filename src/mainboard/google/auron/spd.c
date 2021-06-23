/* SPDX-License-Identifier: GPL-2.0-only */

#include <mainboard/google/auron/variant.h>
#include <soc/pei_wrapper.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->spd_index = variant_get_spd_index();
	spdi->addresses[0] = SPD_MEMORY_DOWN;
	spdi->addresses[2] = variant_is_dual_channel(spdi->spd_index) ? SPD_MEMORY_DOWN : 0;
}
