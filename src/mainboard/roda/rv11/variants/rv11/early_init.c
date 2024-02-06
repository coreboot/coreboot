/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	/* TODO: Confirm if need to enable peg10 in devicetree */
	pei_data->pcie_init = 1;
}
