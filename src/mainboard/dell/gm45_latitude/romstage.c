/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/gm45/gm45.h>

void get_mb_spd_addrmap(u8 spd_addrmap[4])
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x52;
}
