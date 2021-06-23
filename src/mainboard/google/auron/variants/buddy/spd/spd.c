/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/pei_wrapper.h>

void mb_get_spd_map(struct spd_info *spdi)
{
	spdi->addresses[0] = 0x50;
	spdi->addresses[2] = 0x52;
}
