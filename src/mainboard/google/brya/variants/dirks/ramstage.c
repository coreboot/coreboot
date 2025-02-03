/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <soc/ramstage.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *params)
{
	/*Enable Type C port1 convert to Type A*/
	params->EnableTcssCovTypeA[1] = 1;
	/* PCH xchi port number for Type C port1 port mapping */
	params->MappingPchXhciUsbA[1] = 2;

}
