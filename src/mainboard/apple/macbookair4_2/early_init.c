/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <cbfs.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
	{ 1, 0, -1 },
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	void *spd_file;
	size_t spd_file_len = 0;
	spd_file = cbfs_map("spd.bin", &spd_file_len);
	if (spd_file && spd_file_len >= 1024) {
		int i;
		for (i = 0; i < 4; i++)
			memcpy(&spd[i], spd_file + 256 * i, 128);
	}
}
