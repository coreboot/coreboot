/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	params->VccInAuxImonIccImax = 100; // 25 * 4 for ADL-N
	printk(BIOS_INFO, "Override VccInAuxImonIccImax = %d\n", params->VccInAuxImonIccImax);
}
