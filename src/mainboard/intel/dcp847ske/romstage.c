/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	const uint8_t spdaddr[] = {0xa0, 0x00, 0xa2, 0x00};

	memcpy(pei_data->spd_addresses, &spdaddr, sizeof(pei_data->spd_addresses));

	/* TODO: Confirm if nortbridge_fill_pei_data() gets .system_type right (should be 0) */
}
