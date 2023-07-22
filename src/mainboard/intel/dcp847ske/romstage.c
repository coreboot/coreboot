/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <northbridge/intel/sandybridge/raminit.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* TODO: Confirm if nortbridge_fill_pei_data() gets .system_type right (should be 0) */
}
