/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <stdbool.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, 0 },
	{ 1, 0, 0 },
	{ 1, 0, 1 },
	{ 1, 0, 1 },
	{ 1, 0, 2 },
	{ 1, 0, 2 },
	{ 1, 0, 3 },
	{ 1, 0, 3 },
	{ 1, 0, 4 },
	{ 1, 0, 4 },
	{ 1, 0, 6 },
	{ 1, 0, 5 },
	{ 1, 0, 5 },
	{ 1, 0, 6 },
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
