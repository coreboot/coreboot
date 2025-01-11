/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	/* C1S0 is a soldered RAM with no real SPD. Use stored SPD. */
	spdi->addresses[0] = SPD_MEMORY_DOWN;
	spdi->addresses[2] = 0x51;
	spdi->spd_index = 0;
}
