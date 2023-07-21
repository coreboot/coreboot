/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <stdint.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	const uint8_t spdaddr[] = {0xa0, 0x00, 0xa2, 0x00};

	memcpy(pei_data->spd_addresses, &spdaddr, sizeof(pei_data->spd_addresses));
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x51, id_only);
}
