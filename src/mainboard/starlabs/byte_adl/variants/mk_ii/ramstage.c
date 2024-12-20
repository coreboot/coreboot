/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *supd)
{
	/*
	 * FSP defaults to pins that are used for LPC; given that
	 * coreboot only supports eSPI, set these pins accordingly.
	 */
	supd->CnviRfResetPinMux			= 0x194ce404;	// GPP_F4
	supd->CnviClkreqPinMux			= 0x294ce605;	// GPP_F5
	supd->SataPortDevSlpPinMux[1]		= 0x5967400d;	// GPP_H13
}
