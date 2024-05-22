/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	// XXX: Enabling C10 reporting causes system to constantly enter and
	// exit opportunistic suspend when idle.
	params->PchEspiHostC10ReportEnable = 0;
}
