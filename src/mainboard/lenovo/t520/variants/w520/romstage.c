/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit_native.h>

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[1], 0x52, id_only);
	read_spd(&spd[2], 0x51, id_only);
	read_spd(&spd[3], 0x53, id_only);
}
