/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/* Refer to FSP Silicon (soc/intel/alderlake/fsp_params.c)
	   VccIn Aux Imon IccMax. Values are in 1/4 Amp increments */
	params->VccInAuxImonIccImax = 108; /* 30000(30A) * 4 / 1000 */
	printk(BIOS_INFO, "Override VccInAuxImonIccImax = %d\n", params->VccInAuxImonIccImax);
}
